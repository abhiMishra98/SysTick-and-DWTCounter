/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "timingAnalyzer.h"
#include <stdbool.h>
#include <math.h>

volatile unsigned int time = 0;
CY_ISR_PROTO(isr_Timer1ms_terminalCount);
CY_ISR_PROTO(isr_Timer2ms_terminalCount);

static void delay_function(void) {
  
     for (int i = 0; i < 1000; i++) { // Adjusted to 1000 for each set of 1000 operations

        // Uncomment the block of code you want to test.
        int result;
        float resultFloat;
        double resultDouble;
        // Integer additions
        
        result = 0;
        for (int j = 0; j < 1000; j++) {
            result += j;
        }
        

        // Integer multiplications / divisions
        
//        result = 1;
//        for (int j = 1; j <= 1000; j++) {
//            result *= j;
//            result /= j + 1;
//        }
        

        // Float additions
    
//        resultFloat = 0.0f;
//        for (int j = 0; j < 1000; j++) {
//            resultFloat += 1.1f;
//        }
        

        // Float multiplications / divisions
        
//        resultFloat = 1.0f;
//        for (int j = 1; j <= 1000; j++) {
//            resultFloat *= 1.1f;
//            resultFloat /= 1.01f;
//        }
        

        // Square root calculations
        
//        resultFloat = 0.0f;
//        for (int j = 1; j <= 1000; j++) {
//            resultFloat = sqrtf((float)j);
//        }
       

        // Sine calculations with double precision
        
//        resultDouble = 0.0;
//        for (int j = 1; j <= 1000; j++) {
//            resultDouble = sin((double)j);
//        }
        

        // Sine calculations with float precision
        
//        resultFloat = 0.0f;
//        for (int j = 1; j <= 1000; j++) {
//            resultFloat = sinf((float)j);
//        }
        
    }
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    /* Initialize components */
//    isr_Timer1ms_StartEx(isr_Timer1ms_terminalCount);
//    timer_clock_Start();
//    Timer_1ms_Start();
    
//    isr_Timer2ms_StartEx(isr_Timer2ms_terminalCount);
//    timer_clock2ms_Start();
//    Timer_2ms_Start();
    
    UART_LOG_Start();
    TimingAnalyzer_Init();
  
    //Create objects for different analysers.
    TimingAnalyzer_t dwt_analyzer;
    TimingAnalyzer_t dwt_analyzerPin;
    TimingAnalyzer_t systick_analyzer;
    TimingAnalyzer_t systick_analyzerPin;
    TimingAnalyzer_t led_analyzer;
    
    //Create Timing Analyzers for different modes.
    TimingAnalyzer_Create(&systick_analyzer, MODE_SYSTICK, 4, "SysTick Test");  // No LED
    TimingAnalyzer_Create(&systick_analyzerPin, MODE_SYSTICK_PIN, 0, "SysTick Test + Output Pin");  // With Red LED.
    TimingAnalyzer_Create(&dwt_analyzer, MODE_DWT_COUNTER, 4, "DWT Cycle Counter");  // No LED
    TimingAnalyzer_Create(&dwt_analyzerPin, MODE_DWT_COUNTER_PIN, 1, "DWT Cycle Counter + Output Pin");  // With Green LED
    TimingAnalyzer_Create(&led_analyzer, MODE_PIN_ONLY, 2, "LED Test");         // With yellow LED only
    
    
    //Basic test for systick analyzer
    TimingAnalyzer_Start(&systick_analyzer);
   // delay_function(); 
    CyDelay(1000); //5s
    TimingAnalyzer_Stop(&systick_analyzer);
    TimingAnalyzer_PrintSingle(&systick_analyzer);
    
    //Basic test for systick analyzer and Pin.
    TimingAnalyzer_Start(&systick_analyzerPin);
     CyDelay(1000); //5000 ms
    TimingAnalyzer_Stop(&systick_analyzerPin);
    TimingAnalyzer_PrintSingle(&systick_analyzerPin);
     
    //Basic test for DWT cycle counter
    TimingAnalyzer_Start(&dwt_analyzer);
     CyDelay(1000);//5s
    TimingAnalyzer_Stop(&dwt_analyzer);
    TimingAnalyzer_PrintSingle(&dwt_analyzer);
    
    //Basic test for DWT cycle counter and pin
    TimingAnalyzer_Start(&dwt_analyzerPin);
     CyDelay(1000);
    TimingAnalyzer_Stop(&dwt_analyzerPin);
    TimingAnalyzer_PrintSingle(&dwt_analyzerPin);
    
    //Basic test for led analyzer
    TimingAnalyzer_Start(&led_analyzer);
     CyDelay(1000);
    TimingAnalyzer_Stop(&led_analyzer);
    TimingAnalyzer_PrintSingle(&led_analyzer);
  
    //Basic test for systick analyzer
//    UART_LOG_PutString ("\r\n TC1 - Systick Counter \r\n");
//    TimingAnalyzer_Start(&systick_analyzer);
//    CyDelay(600); //600 ms
//    TimingAnalyzer_Pause(&systick_analyzer);
//    CyDelay(600); //600 ms not added.
//    TimingAnalyzer_Resume(&systick_analyzer);
//    CyDelay(600); //600 ms should be added.
//    TimingAnalyzer_Pause(&systick_analyzer);
//    CyDelay(600); //600 ms not added.
//    TimingAnalyzer_Stop(&systick_analyzer);
//    TimingAnalyzer_PrintSingle(&systick_analyzer);
    
    //Basic test for DWT cycle counter
 //     UART_LOG_PutString("\r\n TC2 - DWT Cycle Counter \r\n");
//    TimingAnalyzer_Start(&dwt_analyzer);
//    CyDelay(600); //600 ms
//    TimingAnalyzer_Pause(&dwt_analyzer);
//    CyDelay(600); //600 ms not added.
//    TimingAnalyzer_Resume(&dwt_analyzer);
//     CyDelay(600); //600 ms should be added.
//    TimingAnalyzer_Pause(&dwt_analyzer);
//    CyDelay(600); //600 ms not added.
//    TimingAnalyzer_Stop(&dwt_analyzer);
//    TimingAnalyzer_PrintSingle(&dwt_analyzer);
    
    
    TimingAnalyzer_PrintAll();
    
    
  
}

CY_ISR(isr_Timer1ms_terminalCount){
    UART_LOG_PutChar('A');
    Timer_1ms_ReadStatusRegister();
    isr_Timer1ms_ClearPending();
    
    TimingAnalyzer_t systick_analyzer;
    TimingAnalyzer_Create(&systick_analyzer, MODE_SYSTICK, 4, "SysTick Test");
    
    TimingAnalyzer_Start(&systick_analyzer);
    CyDelay(5000);
    TimingAnalyzer_Stop(&systick_analyzer);
    TimingAnalyzer_PrintSingle(&systick_analyzer); 
}

CY_ISR(isr_Timer2ms_terminalCount){
    UART_LOG_PutChar('B');
    Timer_2ms_ReadStatusRegister();
    isr_Timer2ms_ClearPending();
  
    TimingAnalyzer_t systick_analyzer;
    TimingAnalyzer_Create(&systick_analyzer, MODE_SYSTICK, 4, "SysTick Test");
    
    TimingAnalyzer_Start(&systick_analyzer);
    CyDelay(5000);
    TimingAnalyzer_Stop(&systick_analyzer);
    TimingAnalyzer_PrintSingle(&systick_analyzer); 
}

/* [] END OF FILE */
