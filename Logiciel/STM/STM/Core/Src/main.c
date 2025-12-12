/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "Keyboard.h"
#include "arm_logic.h"
#include "UART_Com.h"
#include "Mem_Tac.h"
#include "Gui.h"
#include "Automanu_mode.h"
#include <stdbool.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TRUE  1
#define FALSE 0

#define OPEN  1
#define CLOSE 0

#define MANUAL 69
#define AUTO 67
#define DANCE 42

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
int function = 0;
Point Table_pos = {0,0};
int Out_Pivots[5];
int test = 0;
int Weight = 0;
int ctrl_mode = AUTO;
int key;

// non blocking delay:
typedef enum {
  STATE_IDLE,
  STATE_AT_HOME,
  STATE_GRAB_WEIGHT,
  STATE_WAIT_1,
  STATE_WAIT_2,
} ArmState;

ArmState arm_state = STATE_IDLE;
int now;
int state_timer = 0;
int adc_weight = 0;
int adc_pince = 0;
Point saved_pos = {0, 0};
int button_timer = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
/* USER CODE BEGIN PFP */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */

  /* Initialize LCD */
  GUI_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

while (1) {
  // ----- run main code if pic received shit----- //
  now = HAL_GetTick();

  // here check * or # button to go to manual or AUTOMANU
  key = Clavier_MX();    
  if ((key == '*') && (now - button_timer >= 1500)) {
    button_timer = now;  // reset debounce timer
    if (ctrl_mode != MANUAL) {
      ctrl_mode = MANUAL;
      arm_state = STATE_IDLE;
    }    
    else {
      ctrl_mode = AUTO;
    }
  }
  // # toggle AUTOMANU)
  if (key == '#') {
    LCD_Clear();
    Automanu_mode();
    arm_state = STATE_IDLE;
  }
  // A to toggle FUN mode 
  if ((key == 'C') && (now - button_timer >= 1500)) {
    button_timer = now;  // reset debounce timer
    if (ctrl_mode != DANCE) {
      ctrl_mode = DANCE;
      ARM_LOGIC(10, 0, 10, CLOSE, Out_Pivots);
      arm_state = STATE_IDLE;
    }    
    else {
      ctrl_mode = AUTO;
    }
  }
  

  // get the 8 bits fromt the pic
  uint8_t* UART_Inputs = UART_Receive();
  Table_pos = Lire_Tab(UART_Inputs);
  
  // display every info and check for manue ctrl 
  adc_weight = ADC_Read_Balance();
  adc_pince = ADC_Read_Pince();
  Run_GUI(Table_pos.x, Table_pos.y, ctrl_mode, Out_Pivots, adc_weight, adc_pince);


  // ----- mode auto -----//
  if (ctrl_mode == AUTO) {
    switch (arm_state) {
    
    // idle logic
    case STATE_IDLE:
      if ((Table_pos.x != 0) || (Table_pos.y != 0)) {
        // Weight detected
        saved_pos = Table_pos;
        state_timer = now;
        arm_state = STATE_GRAB_WEIGHT;
      }
      // if no weight after 5 seconds, go home
      else if (now - state_timer >= 7500) {
        ARM_LOGIC(10, -1.5, 10, CLOSE, Out_Pivots);
        ARM_LOGIC(10, -1.5,  0, CLOSE, Out_Pivots);
        arm_state = STATE_AT_HOME;
      }
      break;

    case STATE_AT_HOME:
      if ((Table_pos.x != 0) || (Table_pos.y != 0)) {
        saved_pos = Table_pos;
        ARM_LOGIC(10, -1.5, 10, CLOSE, Out_Pivots);
        ARM_LOGIC(00, 23.0, 20,  OPEN, Out_Pivots);
        state_timer = now;
        arm_state = STATE_GRAB_WEIGHT;
      }
      break;
    
    // grabs the weight
    case STATE_GRAB_WEIGHT:
      if (now - state_timer >= 750) {
        ARM_LOGIC(saved_pos.x, saved_pos.y, AUTO, CLOSE, Out_Pivots); 
        state_timer = now;
        arm_state = STATE_WAIT_1;
      }
      break;

    // palces the cylinders in the balance
    case STATE_WAIT_1:
      if (now - state_timer >= 1500) {
        ARM_LOGIC(-2, 40, 5, OPEN, Out_Pivots);
        state_timer = now;
        arm_state = STATE_WAIT_2;
      }
      break;
        
    // test for the wieght and the go to its desired section
    case STATE_WAIT_2:
      if (now - state_timer >= 4000) {
        // read wieght and dispaly it
        adc_weight = ADC_Read_Balance();
        adc_pince = ADC_Read_Pince();  
        Run_GUI(Table_pos.x, Table_pos.y, ctrl_mode, Out_Pivots, adc_weight, adc_pince);
        ARM_LOGIC(-2, 40, AUTO, CLOSE, Out_Pivots);
        
        // weight 20G
        if      (adc_weight >=  250 && adc_weight <= 1000) {
          ARM_LOGIC(14, 25, 8,  OPEN, Out_Pivots); 
          ARM_LOGIC(14, 25, 12, OPEN, Out_Pivots); 
          
          // special animation
          ARM_LOGIC(-11, -16, 10, OPEN, Out_Pivots); 
          HAL_Delay(1000);
          ARM_LOGIC(-11, -16, -2.5,  OPEN, Out_Pivots);
          ARM_LOGIC(-11, -15, -2.5, CLOSE, Out_Pivots);
          HAL_Delay(500);
          ARM_LOGIC(-11, -25, -5, CLOSE, Out_Pivots);
        }
        // weight 50G
        else if (adc_weight >= 1000 && adc_weight <= 2000) {
          ARM_LOGIC(15, 29.6, 8, OPEN, Out_Pivots); 
        }
        // weight 80G
        else if (adc_weight >= 2000 && adc_weight <= 3000) {
          ARM_LOGIC(15, 33.5, 8, OPEN, Out_Pivots); 
        }
        // no weight
        else {
          ARM_LOGIC(0, 23, 15, OPEN, Out_Pivots);
          arm_state = STATE_IDLE;
        }

        HAL_Delay(1000);
        state_timer = HAL_GetTick();
        ARM_LOGIC(0, 23, 15, OPEN, Out_Pivots);
        arm_state = STATE_IDLE;
      }
      break;
    }
    HAL_Delay(500);
  }

  // ----- mode manuel -----//
  if (ctrl_mode == MANUAL) {
    // pivot 0
    if (key == '1' && Out_Pivots[0] < 205) Out_Pivots[0] += 2;
    if (key == '4' && Out_Pivots[0] > 000) Out_Pivots[0] -= 2;
    // pivot 1    
    if (key == '2' && Out_Pivots[1] < 205) Out_Pivots[1] += 2;
    if (key == '5' && Out_Pivots[1] > 000) Out_Pivots[1] -= 2;
    // pivot 2
    if (key == '3' && Out_Pivots[2] < 205) Out_Pivots[2] += 2;
    if (key == '6' && Out_Pivots[2] > 000) Out_Pivots[2] -= 2;
    // pivot 3    
    if (key == 'A' && Out_Pivots[3] < 205) Out_Pivots[3] += 2;
    if (key == 'B' && Out_Pivots[3] > 000) Out_Pivots[3] -= 2;
    // pivot 4 (toggle open(Out_Pivots[4] = 0) / close(Out_Pivots[4] = 205))
    if (key == 'C') {
      Out_Pivots[4] = (Out_Pivots[4] == 0) ? 205 : 0;
    }

    UART_Send(
      (uint8_t)Out_Pivots[0],
      (uint8_t)Out_Pivots[1],
      (uint8_t)Out_Pivots[2],
      (uint8_t)Out_Pivots[3],
      (uint8_t)Out_Pivots[4]
    );
  }

  // ----- mode fun -----//
  if (ctrl_mode == DANCE) {   // stupid dance function made by jess
    ARM_LOGIC(-40, 0, 15, OPEN, Out_Pivots); 
    ARM_LOGIC(-10, 0, 15, OPEN, Out_Pivots); 
  }
  
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_41CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 19200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, P1_2_Pin|P1_3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, RW_Pin|EN_Pin|D4_Pin|D5_Pin
                          |D6_Pin|D7_Pin|RS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_BP_Pin */
  GPIO_InitStruct.Pin = LED_BP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LED_BP_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : P1_2_Pin P1_3_Pin */
  GPIO_InitStruct.Pin = P1_2_Pin|P1_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : P1_4_Pin P1_5_Pin P1_6_Pin P1_7_Pin */
  GPIO_InitStruct.Pin = P1_4_Pin|P1_5_Pin|P1_6_Pin|P1_7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : P2_0_Pin P2_6_Pin P2_3_Pin P2_1_Pin
                           P2_2_Pin P2_7_Pin */
  GPIO_InitStruct.Pin = P2_0_Pin|P2_6_Pin|P2_3_Pin|P2_1_Pin
                          |P2_2_Pin|P2_7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : RW_Pin EN_Pin D4_Pin D5_Pin
                           D6_Pin D7_Pin RS_Pin */
  GPIO_InitStruct.Pin = RW_Pin|EN_Pin|D4_Pin|D5_Pin
                          |D6_Pin|D7_Pin|RS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
