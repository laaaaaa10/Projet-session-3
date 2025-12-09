// ************************************************************************* // 
// File: Automanu_mode.c
// Done by : Javier
// Description :
// 3rd mode i did off college for fun
// gpt helped me alot with it cuse idrc abought this usless mode 
// (literraly the middle child) also if ur reading this i hope you get an anal prolapse
// ************************************************************************* // 

// *************************** INCLUDES ************************************ // 
#include "Automanu_mode.h"
#include "lcd.h"
#include "Keyboard.h"
#include "arm_logic.h"
#include <stdio.h>

// *************************** DEFINES ************************************ //
#define TRUE  1
#define FALSE 0

#define OPEN  1
#define CLOSE 0

// *************************** VARIABLES ************************************ //
static int key;

float X_val;
float Y_val;
float Z_val;

int digit_pos;  
int current_coord;  // 0=X, 1=Y, 2=Z

int X_sign; 
int Y_sign; 

int digits[4];
int error_timer;  // Timer for error message display

int last_coord = -1;      // Track last active coordinate
int last_digit_pos = -1;  // Track last digit position
uint32_t last_exit_time = 0;  // Cooldown timer for # button

// ************************* FUCNTIONS ******************************* //

// ----- basic setup shit ----- //
void Display_Coordinate(float value, int is_active, int has_sign, int coord) {
    // Extract digits from value for display
    int whole = (int)value;
    int decimal = (int)((value - whole) * 100);
    
    int d0 = whole / 10;
    int d1 = whole % 10;
    int d2 = decimal / 10;
    int d3 = decimal % 10;
    
    // Display sign if needed (for X and Y)
    if (has_sign) {
        if (coord == 0) {
            if (X_sign > 0) {
                LCD_Print("+");
            }
            else {
                LCD_Print("-");
            }
        }
        else if (coord == 1) {
            if (Y_sign > 0) {
                LCD_Print("+");
            }
            else {
                LCD_Print("-");
            }
        }
    }
    
    // Display with underscores for active input
    if (is_active) {
        if (digit_pos == 0) LCD_Print("_");
        else LCD_PrintInt(d0);
        
        if (digit_pos == 1) LCD_Print("_");
        else LCD_PrintInt(d1);
        
        LCD_Print(".");
        
        if (digit_pos == 2) LCD_Print("_");
        else LCD_PrintInt(d2);
        
        if (digit_pos == 3) LCD_Print("_");
        else LCD_PrintInt(d3);
    }
    else {
        // Just display the value normally
        LCD_PrintInt(d0);
        LCD_PrintInt(d1);
        LCD_Print(".");
        LCD_PrintInt(d2);
        LCD_PrintInt(d3);
    }
}

// ----- main display thing ----- //
uint16_t Automanu_mode(void) {
    last_exit_time = HAL_GetTick();
    HAL_Delay(500);
    
start_over:  // Jump here to restart everything
    
    // Initialize values
    X_val = 0.0;
    Y_val = 0.0;
    Z_val = 0.0;
    digit_pos = 0;
    current_coord = 0;  // Start with X
    
    X_sign = 1;  // Start positive
    Y_sign = 1;  // Start positive
    
    error_timer = 0;  // No error initially
    
    // Clear digits for X
    digits[0] = 0;
    digits[1] = 0;
    digits[2] = 0;
    digits[3] = 0;

    // --- main while loop ---//
    while (1) {
        // lcd clear that wont run every time cause gpt was whining abought it
        if (current_coord != last_coord || 
            digit_pos != last_digit_pos ||
            error_timer == 100 || 
            error_timer == 1) {
            LCD_Clear();
            last_coord = current_coord;
            last_digit_pos = digit_pos;
        }

        // Display X coordinate (LCD line 0)
        LCD_Set(0, 0);
        LCD_Print("X=");
        if (current_coord == 0) {
            Display_Coordinate(X_val, 1, 1, 0);  // active, has sign
        }
        else {
            if (X_sign < 0) LCD_Write('-');
            else LCD_Write('+');
            Display_Coordinate(X_val, 0, 0, 0);  // not active, sign already shown
        }

        // Display Y coordinate (LCD line 1)
        LCD_Set(0, 1);
        LCD_Print("Y=");
        if (current_coord == 1) {
            Display_Coordinate(Y_val, 1, 1, 1);  // active, has sign
        }
        else {
            if (Y_sign < 0) LCD_Write('-');
            else LCD_Write('+');
            Display_Coordinate(Y_val, 0, 0, 1);  // not active, sign already shown
        }

        // Display Z coordinate (LCD line 2)
        LCD_Set(0, 2);
        LCD_Print("Z=");
        if (current_coord == 2) {
            Display_Coordinate(Z_val, 1, 0, 2);  // active, no sign
        }
        else {
            Display_Coordinate(Z_val, 0, 0, 2);  // not active, no sign
        }

        
        // errors n shit (LCD line 3)
        LCD_Set(0, 3);
        if (error_timer > 0) {
            LCD_Print("invalid range");
            error_timer --;           // Count down the error timer 
        }
        else if (current_coord < 2 || digit_pos < 4) {
            LCD_Print("missing digits");
        }
        else {
            LCD_Print("A=Run B=Back C=+/-");
        }


        // Get keyboard press
        key = Clavier_MX();
        
        // Press # to exit back to main
        if (key == '#') {
            // Check cooldown - must wait 2 seconds since function started
            if (HAL_GetTick() - last_exit_time > 2000) {
                // Wait until # key is released
                while (Clavier_MX() == '#') {
                    HAL_Delay(50);
                }
                HAL_Delay(200);
                return 1;  // return to the main.c cause fuck you
            }
            key = 0;  // Ignore if within cooldown period
        }

        // Handle digit input (0-9)
        else if (key >= '0' && key <= '9') {

            digits[digit_pos] = key - '0';    // Store the digit
            digit_pos ++;                     // Move to next position
            
            // Update the value being entered
            float temp = digits[0] * 10 + digits[1] + (digits[2] * 0.1) + (digits[3] * 0.01);
            
            if (current_coord == 0) {
                X_val = temp;    
            }
            else if (current_coord == 1) {
                Y_val = temp;    
            }
            else {
                Z_val = temp;
            }
            
            // If current coordinate is complete, move to next
            if (digit_pos >= 4) {
                if (current_coord < 2) {  // If not on Z yet
                    current_coord ++;     // Move to next coordinate
                    digit_pos = 0;        // Reset position
                    
                    // Clear digits for next coordinate
                    digits[0] = 0;
                    digits[1] = 0;
                    digits[2] = 0;
                    digits[3] = 0;
                }
            }
            key = 0;

            // debounce
            while (Clavier_MX() >= '0' && Clavier_MX() <= '9') {
            HAL_Delay(10);
                }
            HAL_Delay(50);  // Extra debounce delay
        }

        
        // Press A to validate and send coordinates
        else if (key == 'A' && current_coord == 2 && digit_pos >= 4) {
            
            // Apply signs to X and Y
            float final_X = X_val * X_sign;
            float final_Y = Y_val * Y_sign;
            float final_Z = Z_val;
            
            // Check if values are in valid range
            if ((final_X >= -40.0 && final_X <= 40.0) && 
                (final_Y >= -40.0 && final_Y <= 40.0) && 
                (final_Z >=   5.0 && final_Z <= 20.0)) {
                
                LCD_Clear();
                LCD_Print("moving...");
                ARM_LOGIC(final_X, final_Y, final_Z, CLOSE, Out_Pivots);
                HAL_Delay(1500);
                
                // Start over the entire logic  GPT TOLD ME THAT, WTF THAT EXISTS IN C!!! C IS BECOMING ASSEMBLY NOOOOOOOOOOOOOOOOOOOOOOO
                goto start_over;
            }
            else {
                // Invalid range - set error timer
                error_timer = 100;  // Show error for ~100 loop iterations
            }
            
            key = 0;
        }


        // Press C to toggle sign (only for X and Y)
        else if (key == 'C') {
            if (current_coord == 0) {
                X_sign = -X_sign;  // Toggle X sign
            }
            else if (current_coord == 1) {
                Y_sign = -Y_sign;  // Toggle Y sign
            }
            key = 0;
        }


        // Press B to backspace
        else if (key == 'B') {
            if (digit_pos > 0) {
                digit_pos --;            // Go back one position
                digits[digit_pos] = 0;   // Clear the digit

                // Recalculate the value
                float temp = digits[0] * 10 + digits[1] + (digits[2] * 0.1) + (digits[3] * 0.01);

                if (current_coord == 0) {
                    X_val = temp;    
                }
                else if (current_coord == 1) {
                    Y_val = temp;    
                }
                else {
                    Z_val = temp;
                }
            }
            // If nothing to backspace, go to previous coordinate
            else if (digit_pos == 0 && current_coord > 0) {
                current_coord --;        // Go back to previous coordinate
                digit_pos = 4;           // Start at end of that coordinate

                // Restore digits from the previous coordinate value
                float prev_val;
                if (current_coord == 0) {
                    prev_val = X_val;
                }
                else if (current_coord == 1) {
                    prev_val = Y_val;
                }
                else {
                    prev_val = Z_val;
                }

                // Extract digits back
                int whole = (int)prev_val;
                int decimal = (int)((prev_val - whole) * 100);

                digits[0] = whole / 10;
                digits[1] = whole % 10;
                digits[2] = decimal / 10;
                digits[3] = decimal % 10;
            }

            key = 0;
        }
    }
}