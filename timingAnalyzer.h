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

#include "global.h"
#include "project.h"
#include <stdio.h>
#include <stdbool.h>

#define MAX_NAME_LENGTH 32
#define MAX_ANALYZERS 8
typedef enum{
    MODE_DWT_COUNTER = 0,
    MODE_DWT_COUNTER_PIN,
    MODE_SYSTICK,
    MODE_SYSTICK_PIN,
    MODE_PIN_ONLY
}TimerMode_t;

// Function pointer type for pin control
typedef void (*PinControl_t)(uint8_t value);

typedef enum {
    STATE_CONFIGURED,
    STATE_START,
    STATE_STOP,
    STATE_PAUSE,
    STATE_RESUME
} TimerState_t;

typedef struct{
    char name[MAX_NAME_LENGTH];
    TimerMode_t mode;
    uint8_t pin;
    TimerState_t state;
    uint32_t startTime;
    uint32_t endTime;
    PinControl_t pinControl;
    uint32_t totalTime;
    float totalTimeDWT;
    uint32_t cycleCount;
    bool isValid;
} TimingAnalyzer_t;





/***************************************
*    Function Prototypes
***************************************/


/**
 * Will initialise the DWT and SysTick Counters.
 *
 *
 * \return RC_SUCCESS if the initialisation was done.
 */
RC_t TimingAnalyzer_Init(void);



/**
 * Will create the timing analyzer out of the 5 different available options.
 *
 * \param TimingAnalyzer_t *const me : [IN] the object of the timingAnalyzer struct.
 * \param TimerMode_t       		 : [IN] enum for the different modes available.
 * \param uint8_t pin	             : [IN] unsigned 8-bit value for the pin control.
 * \param const char* name           : [IN] name of the analyzer.
 *
 * \return RC_SUCCESS if analyser was created, RC_ERROR in case it was not.
 */
RC_t TimingAnalyzer_Create(TimingAnalyzer_t *const me, TimerMode_t,uint8_t pin,const char* name);


/**
 * Prints the value of elapsed time, cycle counts for the timing analyzer.
 *
 * \param TimingAnalyzer_t *const me : [IN] the object of the timingAnalyzer struct.
 *
 * \return RC_SUCCESS if print was successful.
 */
RC_t TimingAnalyzer_PrintSingle(TimingAnalyzer_t *const me);



/**
 * Prints all the timing analyzers.
 *
 *
 * \return RC_SUCCESS if the memory was allocated, RC_ERROR in case no memory was allocated
 */
RC_t TimingAnalyzer_PrintAll(void);



/**
 * Starts the timing analyzer.
 *
 * \param TimingAnalyzer_t *const me : [IN] the object of the timingAnalyzer struct.
 *
 * \return RC_SUCCESS if the analyser was started only after being configured and not runnnig currently.
 *   If analyzer is paused, it should resume.
 * \return RC_ERROR in case the current mode doesn't satisfy the above conditions.
 */
RC_t TimingAnalyzer_Start(TimingAnalyzer_t* me);



/**
 * Stops the timing analyzer.
 *
 * \param TimingAnalyzer_t *const me : [IN] the object of the timingAnalyzer struct.
 *
 * \return RC_SUCCESS if the analyser was stopped either from a running or a pause condition.
 * \return RC_ERROR in case the current mode doesn't satisfy the above conditions.
 */
RC_t TimingAnalyzer_Stop(TimingAnalyzer_t* me);



/**
 * Pauses the timing analyzer.
 *
 * \param TimingAnalyzer_t *const me : [IN] the object of the timingAnalyzer struct.
 *
 * \return RC_SUCCESS if the analyser was paused from a running condition.
 * \return RC_ERROR in case the current mode doesn't satisfy the above conditions.
 */
RC_t TimingAnalyzer_Pause(TimingAnalyzer_t* me);



/**
 * Resumes the timing analyzer.
 *
 * \param TimingAnalyzer_t *const me : [IN] the object of the timingAnalyzer struct.
 *
 * \return RC_SUCCESS if the analyser was resumed from a pause condition.
 * \return RC_ERROR in case the current mode doesn't satisfy the above conditions.
 */
RC_t TimingAnalyzer_Resume(TimingAnalyzer_t* me);


/**
 * Turns the Red LED ON/OFF.
 *
 * \param uint8_t value : [IN] HIGH/LOW value to the LED Pin.
 * 
 */
void TimingAnalyzer_RedLED(uint8_t value);


/**
 * Turns the Green LED ON/OFF.
 *
 * \param uint8_t value : [IN] HIGH/LOW value to the LED Pin.
 *
 */
void TimingAnalyzer_GreenLED(uint8_t value);


/**
 * Turns the Yellow LED ON/OFF.
 *
 * \param uint8_t value : [IN] HIGH/LOW value to the LED Pin.
 *
 * 
 */
void TimingAnalyzer_YellowLED(uint8_t value);


/* [] END OF FILE */
