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
#include <stdio.h>
#include <string.h>

static TimingAnalyzer_t analyzers[MAX_ANALYZERS];
static volatile uint32_t systickCounter = 0;
static uint32_t analyzerCount = 0;

void SysTick_Handler(void) {
    systickCounter++;
}

// Pin control function pointers
static PinControl_t redLedControl;
static PinControl_t greenLedControl;
static PinControl_t yellowLedControl;


/*******************************************************************************
* Function Name: TimingAnalyzer_Init
****************************************************************************//**
*
 * Will initialise the DWT and SysTick Counters.
 *
 *
 * \return RC_SUCCESS if the initialisation was done.
*
*******************************************************************************/
RC_t TimingAnalyzer_Init(){
    //Initializing Systick counter.
    CySysTickInit();
    CySysTickSetCallback(0,SysTick_Handler);
    CySysTickSetReload(BCLK__BUS_CLK__HZ/1000 -1);
    
    //Initializing DWT counter
    CoreDebug-> DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
    return RC_SUCCESS;
}


/*******************************************************************************
* Function Name: TimingAnalyzer_Create
****************************************************************************//**
*
 * Starts the timing analyzer.
 *
 * \param TimingAnalyzer_t *const me : [IN] the object of the timingAnalyzer struct.
 * \param TimerMode_t       		 : [IN] enum for the different modes available.
 * \param uint8_t pin	             : [IN] unsigned 8-bit value for the pin control.
 * \param const char* name           : [IN] name of the analyzer.
 *
 * \return RC_SUCCESS if analyser was created, RC_ERROR in case it was not.
*
*******************************************************************************/
RC_t TimingAnalyzer_Create(TimingAnalyzer_t *const me, TimerMode_t mode,uint8_t pin,const char* name){   
    if(!me){
        return RC_ERROR;      
    }else{
        strcpy(me->name,name);
        me->mode = mode;
        me->pin = pin;
        me->state=STATE_CONFIGURED;
        me->startTime = 0;
        me->endTime = 0;
        me->totalTime = 0;
        me->cycleCount = 0;
   
        // Assign pin control based on pin number
        switch(pin) {
            case 0: me->pinControl = TimingAnalyzer_RedLED;
            break;
            case 1: me->pinControl = TimingAnalyzer_GreenLED;
            // UART_LOG_PutString("Het");
            break;
            case 2: me->pinControl = TimingAnalyzer_YellowLED; 
            break;
            default: me->pinControl = NULL;
        }
        me->isValid = true;
        return RC_SUCCESS;
    }
}


/*******************************************************************************
* Function Name: TimingAnalyzer_Start
****************************************************************************//**
*
 * Starts the timing analyzer.
*
 * \param TimingAnalyzer_t *const me : [IN] the object of the timingAnalyzer struct.
*
 * \return RC_SUCCESS if the analyser was started only after being configured and not runnnig currently.
 *   If analyzer is paused, it should resume.
 * \return RC_ERROR in case the current mode doesn't satisfy the above conditions.
*
*******************************************************************************/

RC_t TimingAnalyzer_Start(TimingAnalyzer_t *const me){
    if(me->state == 0){
        if(me->mode == MODE_DWT_COUNTER || me->mode == MODE_DWT_COUNTER_PIN){
             DWT->CYCCNT =0; // Reset the counter at start
             me->startTime = DWT->CYCCNT;
        }else if (me->mode == MODE_SYSTICK || me->mode == MODE_SYSTICK_PIN) {
            CySysTickEnable();
            systickCounter = 0;  // Reset the counter at start
            me->startTime = systickCounter;
        }
         me->state = STATE_START;
        // Set pin if applicable
        if (me->pinControl && (me->mode == MODE_DWT_COUNTER_PIN || 
                              me->mode == MODE_SYSTICK_PIN || 
                              me->mode == MODE_PIN_ONLY)) {
            me->pinControl(1);
        }
        me->state = STATE_START;          
        return RC_SUCCESS;
    }
    else
    {
        UART_LOG_PutString("Invalid State\r\n");
        return RC_ERROR;
    }
    
}


/*******************************************************************************
* Function Name: TimingAnalyzer_Stop
****************************************************************************//**
*
 * Stops the timing analyzer.
 *
 * \param TimingAnalyzer_t *const me : [IN] the object of the timingAnalyzer struct.
 *
 * \return RC_SUCCESS if the analyser was stopped either from a running or a pause condition.
 * \return RC_ERROR in case the current mode doesn't satisfy the above conditions.
*
*******************************************************************************/
RC_t TimingAnalyzer_Stop(TimingAnalyzer_t* me){
    if(me->state == 1 || me->state == 3){
        if (me->mode == MODE_DWT_COUNTER || me->mode == MODE_DWT_COUNTER_PIN) {
             me->cycleCount+=me->startTime+DWT->CYCCNT;
             float ms = me->cycleCount / (float)(BCLK__BUS_CLK__HZ/1000);
             me->totalTimeDWT = ms;
        } else if (me->mode == MODE_SYSTICK || me->mode == MODE_SYSTICK_PIN) {
            me->totalTime+=me->startTime+systickCounter;
        }
      
        if (me->pinControl && (me->mode == MODE_DWT_COUNTER_PIN || 
                              me->mode == MODE_SYSTICK_PIN || 
                              me->mode == MODE_PIN_ONLY)) 
        {
            me->pinControl(0);
        }
        me->state = STATE_STOP;   
        analyzers[analyzerCount] = *me;
        analyzerCount ++;
        return RC_SUCCESS;
    }
    else
    {
        UART_LOG_PutString("Invalid State\r\n");
        return RC_ERROR;
    }
}


/*******************************************************************************
* Function Name: TimingAnalyzer_Pause
****************************************************************************//**
*
 * Pauses the timing analyzer.
 *
 * \param TimingAnalyzer_t *const me : [IN] the object of the timingAnalyzer struct.
 *
 * \return RC_SUCCESS if the analyser was paused from a running condition.
 * \return RC_ERROR in case the current mode doesn't satisfy the above conditions.
*
*******************************************************************************/
RC_t TimingAnalyzer_Pause(TimingAnalyzer_t* me){
    if(me->state == 1){
        if (me->mode == MODE_DWT_COUNTER || me->mode == MODE_DWT_COUNTER_PIN) {
            DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //Negating the mask and doing an AND operation with the CTRL register to disable the counter.
            me->startTime += DWT->CYCCNT;
            DWT->CYCCNT = 0;
        } else if (me->mode == MODE_SYSTICK || me->mode == MODE_SYSTICK_PIN) {
            CySysTickStop();
            me->startTime += systickCounter;
            systickCounter = 0;
        }
      
        if (me->pinControl && (me->mode == MODE_DWT_COUNTER_PIN || 
                              me->mode == MODE_SYSTICK_PIN || 
                              me->mode == MODE_PIN_ONLY)) {
            me->pinControl(0);
        }
        me->state = STATE_PAUSE;   
        return RC_SUCCESS;
    }
    else
    {
        UART_LOG_PutString("Invalid State\r\n");
        return RC_ERROR;
    }
}


/*******************************************************************************
* Function Name: TimingAnalyzer_Resume
****************************************************************************//**
*
 * Resumes the timing analyzer.
 *
 * \param TimingAnalyzer_t *const me : [IN] the object of the timingAnalyzer struct.
 *
 * \return RC_SUCCESS if the analyser was resumed from a pause condition.
 * \return RC_ERROR in case the current mode doesn't satisfy the above conditions.
*
*******************************************************************************/
RC_t TimingAnalyzer_Resume(TimingAnalyzer_t* me){
    if(me->state == 3){
        if (me->mode == MODE_DWT_COUNTER || me->mode == MODE_DWT_COUNTER_PIN) {
            DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;  //Enabling the clock by an OR operation with the CTRL register.
        } else if (me->mode == MODE_SYSTICK || me->mode == MODE_SYSTICK_PIN) {
            CySysTickEnable();
        }
      
        if (me->pinControl && (me->mode == MODE_DWT_COUNTER_PIN || 
                              me->mode == MODE_SYSTICK_PIN || 
                              me->mode == MODE_PIN_ONLY)) {
            me->pinControl(1);
        }
        me->state = STATE_START;   
        return RC_SUCCESS;
    }
    else
    {
        UART_LOG_PutString("Invalid State\r\n");
        return RC_ERROR;
    }
}


/*******************************************************************************
* Function Name: TimingAnalyzer_PrintSingle
****************************************************************************//**
*
 * Resumes the timing analyzer.
 *
 * \param TimingAnalyzer_t *const me : [IN] the object of the timingAnalyzer struct.
 *
 * \return RC_SUCCESS if print was successful.
*
*******************************************************************************/
RC_t TimingAnalyzer_PrintSingle(TimingAnalyzer_t *const me){
    char buffer[128] = {0};
    UART_LOG_PutString("\r\n");
    UART_LOG_PutString(me->name);     //Prints the name of the timing analyzer.
    
    // Prints the timing information based on mode.
    if (me->mode == MODE_DWT_COUNTER || me->mode == MODE_DWT_COUNTER_PIN){   
     sprintf(buffer + strlen(buffer),"\r\n%s :","Elapsed CPU Cycles"); 
     sprintf(buffer + strlen(buffer), "%u", me->cycleCount);
     sprintf(buffer + strlen(buffer),"\r\n%s :","Elapsed time");
     sprintf(buffer + strlen(buffer), "%d.%06d ms \r\n",(int)me->totalTimeDWT, (int)((me->totalTimeDWT - (int)me->totalTimeDWT) * 1000000));
    } 
    else if (me->mode == MODE_SYSTICK || me->mode == MODE_SYSTICK_PIN) 
    {
        sprintf(buffer + strlen(buffer),"\r\n%s :","Elapsed time"); 
        sprintf(buffer + strlen(buffer), "%d ms\r\n", me->totalTime);
    }
    UART_LOG_PutString(buffer);
    return RC_SUCCESS;
}


/*******************************************************************************
* Function Name: TimingAnalyzer_PrintAll
****************************************************************************//**
*
 * Prints the timing analyzer.
 *
 *
 * \return RC_SUCCESS if print was successful.
*
*******************************************************************************/
RC_t TimingAnalyzer_PrintAll(void) {
    
    UART_LOG_PutString ("\r\n Printing all analyzer data : \r\n");
    for (uint32_t i = 0; i < MAX_ANALYZERS; i++) {
        if (analyzers[i].isValid) {
            TimingAnalyzer_PrintSingle(&analyzers[i]);
        }
    }
    return RC_SUCCESS;
}


/*******************************************************************************
* Function Name: TimingAnalyzer_RedLED
****************************************************************************//**
*
 * Turns the Red LED ON/OFF.
 *
 *
 * \return RC_SUCCESS if print was successful.
*
*******************************************************************************/
void TimingAnalyzer_RedLED(uint8_t value){
    if(value){
        Red_LED_Write(value);
    }
    else{
        Red_LED_Write(0);
    }
}


/*******************************************************************************
* Function Name: TimingAnalyzer_GreenLED
****************************************************************************//**
*
 * Turns the Green LED ON/OFF.
 *
 *
*
*******************************************************************************/
void TimingAnalyzer_GreenLED(uint8_t value){
    if(value){
        Green_LED_Write(value);
    }
    else{
        Green_LED_Write(0);
    }
}


/*******************************************************************************
* Function Name: TimingAnalyzer_YellowLED
****************************************************************************//**
*
 * Turns the Yellow LED ON/OFF.
 *
 *
*
*******************************************************************************/
void TimingAnalyzer_YellowLED(uint8_t value){
    if(value){
        Yellow_LED_Write(value);
    }
    else{
        Yellow_LED_Write(0);
    }
}


/* [] END OF FILE */
