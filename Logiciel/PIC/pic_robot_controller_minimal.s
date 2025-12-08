;===============================================================================
; PIC16F88 Robot Controller
;===============================================================================

    LIST        P=16F88
    #include    <p16f88.inc>

;===============================================================================
; Configuration Bits
;===============================================================================
    __CONFIG    _CONFIG1, _CP_OFF & _CCP1_RB0 & _DEBUG_OFF & _WRT_PROTECT_OFF & _CPD_OFF & _LVP_OFF & _BODEN_OFF & _MCLR_ON & _PWRTE_ON & _WDT_OFF & _XT_OSC
    __CONFIG    _CONFIG2, _IESO_OFF & _FCMEN_OFF

;===============================================================================
; RAM Variables
;===============================================================================
CIRC_BUFFER     EQU     0x20
DELAY_CNT1      EQU     0x31
DELAY_CNT2      EQU     0x32
DELAY_SHORT_CNT EQU     0x33
DELAY_MED_CNT   EQU     0x34
DELAY_LONG_CNT  EQU     0x35
DELAY_VLONG_CNT EQU     0x36
ADC_SELECT      EQU     0x37
ADC_RESULT      EQU     0x38
SENSOR_X        EQU     0x39
SENSOR_Y        EQU     0x3A
SENSOR_PINCE    EQU     0x3B
SENSOR_BALANCE  EQU     0x3C
UART_RX_CHAR    EQU     0x3D
HEX_TEMP        EQU     0x3E
HEX_HIGH        EQU     0x3F
HEX_LOW         EQU     0x40
I2C_FLAGS       EQU     0x41
I2C_DATA_RX     EQU     0x42
I2C_DATA_TX     EQU     0x43
I2C_TEMP        EQU     0x44
I2C_DUMMY1      EQU     0x45
I2C_BIT_CNT     EQU     0x46
I2C_REG_ADDR    EQU     0x47
I2C_REG_DATA    EQU     0x48
SERVO_CHANNEL   EQU     0x4A
MOTOR0_POS      EQU     0x4B
MOTOR1_POS      EQU     0x4C
MOTOR2_POS      EQU     0x4D
MOTOR3_POS      EQU     0x4E
MOTOR4_POS      EQU     0x4F
SERVO_PWM_VAL   EQU     0x50
SERVO_PWM_HIGH  EQU     0x51
CHECKSUM        EQU     0x52
BUF_WRITE_PTR   EQU     0x53
BUF_READ_PTR    EQU     0x54
PARSE_STATE     EQU     0x55
ISR_W_SAVE      EQU     0x71
ISR_STATUS_SAVE EQU     0x72
ISR_PCLATH_SAVE EQU     0x73

;===============================================================================
; Hardware Definitions
;===============================================================================
#define I2C_SCL         PORTB,0
#define I2C_SDA         PORTB,1
#define I2C_SCL_TRIS    TRISB,0
#define I2C_SDA_TRIS    TRISB,1
#define LED_ACK         PORTB,3
#define LED_RX          PORTB,4

PCA9685_ADDR_W  EQU     0xA0
PCA9685_ADDR_R  EQU     0xA1
PCA_MODE1       EQU     0x00
PCA_PRESCALE    EQU     0xFE
PCA_LED0_ON_L   EQU     0x06
FRAME_START1    EQU     'G'
FRAME_START2    EQU     'O'
FRAME_MARKER    EQU     'X'

;===============================================================================
; Reset Vector
;===============================================================================
    ORG         0x000
    clrf        PCLATH
    goto        Main

    ORG         0x002
    retlw       0x00
    retlw       0x00

;===============================================================================
; Interrupt Vector
;===============================================================================
    ORG         0x004
    goto        ISR_Handler

;===============================================================================
; PIC Initialization
;===============================================================================
InitPIC:
    bcf         STATUS,RP1
    banksel     TRISB
    clrf        INTCON
    clrf        ANSEL
    movlw       0x78
    movwf       OPTION_REG
    movlw       0xFF
    movwf       TRISA
    movlw       0xE7
    movwf       TRISB
    banksel     PORTA
    return

;===============================================================================
; UART Functions
;===============================================================================
UART_Init:
    movlw       0x90
    movwf       RCSTA
    banksel     TXSTA
    movlw       0x24
    movwf       TXSTA
    movlw       0x19
    movwf       SPBRG
    banksel     PORTA
    return

UART_ReadWait:
    btfss       PIR1,RCIF
    goto        UART_ReadWait
    movf        RCREG,W
    movwf       UART_RX_CHAR
    return

UART_Write:
    btfss       PIR1,TXIF
    goto        UART_Write
    movwf       TXREG
    return

;===============================================================================
; Delay Functions
;===============================================================================
Delay_NOP:
    nop
    nop
    nop
    nop
    nop
    nop
    return

Delay_Short:
    movlw       0x4B
    movwf       DELAY_SHORT_CNT
Delay_Short_Loop:
    call        Delay_NOP
    decfsz      DELAY_SHORT_CNT,F
    goto        Delay_Short_Loop
    return

Delay_Medium:
    movlw       0x9A
    movwf       DELAY_MED_CNT
Delay_Medium_Loop:
    call        Delay_NOP
    decfsz      DELAY_MED_CNT,F
    goto        Delay_Medium_Loop
    return

Delay_Long:
    movlw       0x05
    movwf       DELAY_LONG_CNT
Delay_Long_Loop:
    call        Delay_Medium
    decfsz      DELAY_LONG_CNT,F
    goto        Delay_Long_Loop
    return

Delay_VeryLong:
    movlw       0xC8
    movwf       DELAY_VLONG_CNT
Delay_VeryLong_Loop:
    call        Delay_Long
    decfsz      DELAY_VLONG_CNT,F
    goto        Delay_VeryLong_Loop
    return

;===============================================================================
; I2C Bit-Bang Functions
;===============================================================================
I2C_SDA_Low:
    bcf         I2C_SDA
    banksel     TRISB
    bcf         I2C_SDA_TRIS
    banksel     PORTA
    return

I2C_SDA_High:
    banksel     TRISB
    bsf         I2C_SDA_TRIS
    banksel     PORTA
    return

I2C_SCL_Low:
    bcf         I2C_SCL
    banksel     TRISB
    bcf         I2C_SCL_TRIS
    banksel     PORTA
    return

I2C_SCL_High:
    banksel     TRISB
    bsf         I2C_SCL_TRIS
    banksel     PORTA
    return

I2C_Start:
    call        I2C_SDA_High
    call        I2C_SCL_High
    call        I2C_SDA_Low
    call        I2C_SCL_Low
    return

I2C_Stop:
    call        I2C_SDA_Low
    call        I2C_SCL_High
    call        I2C_SDA_High
    return

I2C_ReadBit:
    bsf         I2C_FLAGS,7
    call        I2C_SDA_High
    call        I2C_SCL_High
    call        Delay_NOP
    btfss       I2C_SDA
    bcf         I2C_FLAGS,7
    call        I2C_SCL_Low
    return

I2C_WriteBit:
    btfss       I2C_FLAGS,6
    goto        I2C_WriteBit_Low
    call        I2C_SDA_High
    goto        I2C_WriteBit_Clock
I2C_WriteBit_Low:
    call        I2C_SDA_Low
I2C_WriteBit_Clock:
    call        I2C_SCL_High
    call        Delay_NOP
    call        I2C_SCL_Low
    return

I2C_ReadByte:
    clrf        I2C_DATA_RX
    movlw       0x08
    movwf       I2C_BIT_CNT
I2C_ReadByte_Loop:
    bcf         STATUS,C
    rlf         I2C_DATA_RX,F
    call        I2C_ReadBit
    btfsc       I2C_FLAGS,7
    bsf         I2C_DATA_RX,0
    decfsz      I2C_BIT_CNT,F
    goto        I2C_ReadByte_Loop
    return

I2C_WriteByte:
    movlw       0x08
    movwf       I2C_BIT_CNT
I2C_WriteByte_Loop:
    bcf         I2C_FLAGS,6
    btfsc       I2C_DATA_TX,7
    bsf         I2C_FLAGS,6
    call        I2C_WriteBit
    rlf         I2C_DATA_TX,F
    decfsz      I2C_BIT_CNT,F
    goto        I2C_WriteByte_Loop
    call        I2C_ReadBit
    btfsc       I2C_FLAGS,7
    bsf         LED_ACK
    return

;===============================================================================
; I2C Register Access
;===============================================================================
I2C_WriteReg:
    call        I2C_Start
    movlw       PCA9685_ADDR_W
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    movf        I2C_REG_ADDR,W
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    movf        I2C_REG_DATA,W
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    call        I2C_Stop
    call        Delay_Medium
    call        Delay_Medium
    call        Delay_Medium
    call        Delay_Medium
    call        Delay_Medium
    return

I2C_ReadReg:
    call        I2C_Start
    movlw       PCA9685_ADDR_W
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    movf        I2C_REG_ADDR,W
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    call        I2C_Start
    movlw       PCA9685_ADDR_R
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    call        I2C_ReadByte
    bsf         I2C_FLAGS,6
    call        I2C_WriteBit
    call        I2C_Stop
    call        Delay_NOP
    call        Delay_NOP
    call        Delay_NOP
    call        Delay_NOP
    call        Delay_NOP
    return


;===============================================================================
; ADC Functions
;===============================================================================
LireMembraneTactile:
    btfsc       ADC_SELECT,0
    goto        LireMembrane_Ch1
    goto        LireMembrane_Ch0

LireMembrane_Ch0:
    bsf         PORTA,0
    bcf         PORTA,6
    banksel     TRISA
    bsf         ANSEL,1
    bcf         TRISA,0
    bcf         TRISA,6
    banksel     PORTA
    movlw       0x89
    movwf       ADCON0
    goto        LireMembrane_Convert

LireMembrane_Ch1:
    bsf         PORTA,1
    bcf         PORTA,7
    banksel     TRISA
    bsf         ANSEL,0
    bcf         TRISA,7
    bcf         TRISA,1
    banksel     PORTA
    movlw       0x81
    movwf       ADCON0

LireMembrane_Convert:
    call        Delay_Medium
    bsf         ADCON0,GO
LireMembrane_Wait:
    btfsc       ADCON0,GO
    goto        LireMembrane_Wait
    movf        ADRESH,W
    movwf       ADC_RESULT
    banksel     TRISA
    bsf         TRISA,0
    bsf         TRISA,6
    bsf         TRISA,1
    bsf         TRISA,7
    bcf         ANSEL,0
    bcf         ANSEL,1
    banksel     PORTA
    return

LirePince:
    banksel     TRISA
    bsf         ANSEL,2
    banksel     PORTA
    movlw       0x91
    movwf       ADCON0
    call        Delay_Medium
    bsf         ADCON0,GO
LirePince_Wait:
    btfsc       ADCON0,GO
    goto        LirePince_Wait
    movf        ADRESH,W
    movwf       SENSOR_PINCE
    banksel     TRISA
    bcf         ANSEL,2
    banksel     PORTA
    return

LireBalance:
    banksel     TRISA
    bsf         ANSEL,3
    banksel     PORTA
    movlw       0x99
    movwf       ADCON0
    call        Delay_Medium
    bsf         ADCON0,GO
LireBalance_Wait:
    btfsc       ADCON0,GO
    goto        LireBalance_Wait
    movf        ADRESH,W
    movwf       SENSOR_BALANCE
    banksel     TRISA
    bcf         ANSEL,3
    banksel     PORTA
    return

;===============================================================================
; PCA9685 Functions
;===============================================================================
I2CResetPCA9685:
    call        I2C_Start
    movlw       0x00
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    movlw       0x06
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    call        I2C_Stop
    call        Delay_Medium
    return

I2CSetFreqPCA9685:
    call        I2C_Start
    movlw       PCA9685_ADDR_W
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    movlw       PCA_MODE1
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    movlw       0x10
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    call        I2C_Stop

    call        I2C_Start
    movlw       PCA9685_ADDR_W
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    movlw       PCA_PRESCALE
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    movlw       0x7F
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    call        I2C_Stop

    call        I2C_Start
    movlw       PCA9685_ADDR_W
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    movlw       PCA_MODE1
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    movlw       0xA0
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    call        I2C_Stop
    return

;===============================================================================
; Servo Control
;===============================================================================
SetServoPosition:
    call        I2C_Start
    movlw       PCA9685_ADDR_W
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    bcf         STATUS,C
    rlf         SERVO_CHANNEL,F
    bcf         STATUS,C
    rlf         SERVO_CHANNEL,F
    movlw       PCA_LED0_ON_L
    addwf       SERVO_CHANNEL,W
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    movlw       0x00
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    movlw       0x00
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    clrf        SERVO_PWM_HIGH
    movlw       0xCD
    addwf       SERVO_PWM_VAL,W
    btfsc       STATUS,C
    bsf         SERVO_PWM_HIGH,0
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    movf        SERVO_PWM_HIGH,W
    movwf       I2C_DATA_TX
    call        I2C_WriteByte
    call        I2C_Stop
    return

UpdateAllServos:
    movlw       0x00
    movwf       SERVO_CHANNEL
    movf        MOTOR0_POS,W
    movwf       SERVO_PWM_VAL
    call        SetServoPosition

    movlw       0x01
    movwf       SERVO_CHANNEL
    movf        MOTOR1_POS,W
    movwf       SERVO_PWM_VAL
    call        SetServoPosition

    movlw       0x02
    movwf       SERVO_CHANNEL
    movf        MOTOR2_POS,W
    movwf       SERVO_PWM_VAL
    call        SetServoPosition

    movlw       0x03
    movwf       SERVO_CHANNEL
    movf        MOTOR3_POS,W
    movwf       SERVO_PWM_VAL
    call        SetServoPosition

    movlw       0x04
    movwf       SERVO_CHANNEL
    movf        MOTOR4_POS,W
    movwf       SERVO_PWM_VAL
    call        SetServoPosition
    return

;===============================================================================
; Transmit Frame
;===============================================================================
TransmetTrame:
    movlw       0x00
    addlw       FRAME_START1
    addlw       FRAME_START2
    addwf       SENSOR_X,W
    addwf       SENSOR_Y,W
    addwf       SENSOR_PINCE,W
    addwf       SENSOR_BALANCE,W
    addlw       FRAME_MARKER
    movwf       CHECKSUM

    movlw       FRAME_START1
    call        UART_Write
    movlw       FRAME_START2
    call        UART_Write
    movf        SENSOR_X,W
    call        UART_Write
    movf        SENSOR_Y,W
    call        UART_Write
    movf        SENSOR_PINCE,W
    call        UART_Write
    movf        SENSOR_BALANCE,W
    call        UART_Write
    movlw       FRAME_MARKER
    call        UART_Write
    movf        CHECKSUM,W
    call        UART_Write
    return

;===============================================================================
; Main Program
;===============================================================================
Main:
    call        InitPIC
    call        UART_Init
    call        I2CResetPCA9685
    call        I2CSetFreqPCA9685

    clrf        BUF_WRITE_PTR
    clrf        BUF_READ_PTR
    clrf        PARSE_STATE

    banksel     PIE1
    bsf         INTCON,PEIE
    bsf         PIE1,RCIE
    banksel     PORTA
    bsf         INTCON,GIE
    bcf         LED_RX

;===============================================================================
; Main Loop
;===============================================================================
MainLoop:    

    movlw       0x00
    movwf       DELAY_CNT1
    movlw       0x59
    movwf       DELAY_CNT2

    decfsz      DELAY_CNT1,F
    goto        ProcessRxBuffer
    decfsz      DELAY_CNT2,F
    goto        ProcessRxBuffer


    btfsc       LED_ACK
    goto        LED_TurnOff
    goto        LED_TurnOn
LED_TurnOff:
    bcf         LED_ACK
    goto        LED_Done
LED_TurnOn:
    bsf         LED_ACK
LED_Done:

    movlw       0x00
    movwf       ADC_SELECT
    call        LireMembraneTactile
    movf        ADC_RESULT,W
    movwf       SENSOR_Y

    movlw       0x01
    movwf       ADC_SELECT
    call        LireMembraneTactile
    movf        ADC_RESULT,W
    movwf       SENSOR_X

    call        LirePince
    call        LireBalance
    call        TransmetTrame
    goto        MainLoop

;===============================================================================
; RX Buffer Processing / State Machine
;===============================================================================
ProcessRxBuffer:
    movf        BUF_WRITE_PTR,W
    subwf       BUF_READ_PTR,W
    btfsc       STATUS,Z
    goto        MainLoop

    movf        BUF_READ_PTR,W
    andlw       0x07
    addlw       CIRC_BUFFER
    movwf       FSR
    movf        INDF,W
    movwf       UART_RX_CHAR

    incf        BUF_READ_PTR,F
    movlw       0x07
    andwf       BUF_READ_PTR,F

    movlw       0x20
    movwf       INDF

State0_Check:
    movlw       0x00
    subwf       PARSE_STATE,W
    btfss       STATUS,Z
    goto        State1_Check
    movlw       FRAME_START1
    subwf       UART_RX_CHAR,W
    btfss       STATUS,Z
    goto        MainLoop
    incf        PARSE_STATE,F
    movlw       0x07
    andwf       PARSE_STATE,F
    goto        MainLoop

State1_Check:
    movlw       0x01
    subwf       PARSE_STATE,W
    btfss       STATUS,Z
    goto        State2_Check
    movlw       FRAME_START2
    subwf       UART_RX_CHAR,W
    btfss       STATUS,Z
    goto        StateReset
    incf        PARSE_STATE,F
    movlw       0x07
    andwf       PARSE_STATE,F
    goto        MainLoop

State2_Check:
    movlw       0x02
    subwf       PARSE_STATE,W
    btfss       STATUS,Z
    goto        State3_Check
    movf        UART_RX_CHAR,W
    movwf       MOTOR0_POS
    incf        PARSE_STATE,F
    movlw       0x07
    andwf       PARSE_STATE,F
    goto        MainLoop

State3_Check:
    movlw       0x03
    subwf       PARSE_STATE,W
    btfss       STATUS,Z
    goto        State4_Check
    movf        UART_RX_CHAR,W
    movwf       MOTOR1_POS
    incf        PARSE_STATE,F
    movlw       0x07
    andwf       PARSE_STATE,F
    goto        MainLoop

State4_Check:
    movlw       0x04
    subwf       PARSE_STATE,W
    btfss       STATUS,Z
    goto        State5_Check
    movf        UART_RX_CHAR,W
    movwf       MOTOR2_POS
    incf        PARSE_STATE,F
    movlw       0x07
    andwf       PARSE_STATE,F
    goto        MainLoop

State5_Check:
    movlw       0x05
    subwf       PARSE_STATE,W
    btfss       STATUS,Z
    goto        State6_Check
    movf        UART_RX_CHAR,W
    movwf       MOTOR3_POS
    incf        PARSE_STATE,F
    movlw       0x07
    andwf       PARSE_STATE,F
    goto        MainLoop

State6_Check:
    movlw       0x06
    subwf       PARSE_STATE,W
    btfss       STATUS,Z
    goto        State7_Check
    movf        UART_RX_CHAR,W
    movwf       MOTOR4_POS
    incf        PARSE_STATE,F
    movlw       0x07
    andwf       PARSE_STATE,F
    goto        MainLoop

State7_Check:
    movlw       0x07
    subwf       PARSE_STATE,W
    btfss       STATUS,Z
    goto        State0_Check
    movf        UART_RX_CHAR,W
    movwf       CHECKSUM
    incf        PARSE_STATE,F
    movlw       0x07
    andwf       PARSE_STATE,F
    call        UpdateAllServos
    goto        MainLoop

StateReset:
    clrf        PARSE_STATE
    goto        MainLoop

;===============================================================================
; ISR
;===============================================================================
    ORG         0x2FE
ISR_Handler:
    movwf       ISR_W_SAVE
    movf        STATUS,W
    movwf       ISR_STATUS_SAVE
    movf        PCLATH,W
    movwf       ISR_PCLATH_SAVE

    btfss       PIR1,RCIF
    goto        ISR_Exit

    movf        RCREG,W
    movwf       UART_RX_CHAR
    bcf         PIR1,RCIF

    movf        BUF_WRITE_PTR,W
    andlw       0x07
    addlw       CIRC_BUFFER
    movwf       FSR
    movf        UART_RX_CHAR,W
    movwf       INDF

    incf        BUF_WRITE_PTR,F
    movlw       0x07
    andwf       BUF_WRITE_PTR,F

    btfsc       LED_RX
    goto        ISR_LED_Off
    goto        ISR_LED_On
ISR_LED_Off:
    bcf         LED_RX
    goto        ISR_Exit
ISR_LED_On:
    bsf         LED_RX

ISR_Exit:
    movf        ISR_PCLATH_SAVE,W
    movwf       PCLATH
    movf        ISR_STATUS_SAVE,W
    movwf       STATUS
    swapf       ISR_W_SAVE,F
    swapf       ISR_W_SAVE,W
    retfie

;===============================================================================
    END
