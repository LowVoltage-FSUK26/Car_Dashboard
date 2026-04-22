#ifndef PROX_INT_H
#define PROX_INT_H


typedef struct
{
    int32_t curr;
    int32_t prev;
    int32_t diff;

    int32_t Acculmulated_Overflow_Count;
    int32_t Actual_Overflow_Count;
    uint8_t velocity;

    double time;
    double After_Filtering; // Testing
} Prox_Wheel_Velcoity;


#define     FRONT_LEFT      0
#define     FRONT_RIGHT     1
#define     REAR_LEFT       2
#define     REAR_RIGHT      3


void PROX_INIT();
void PROX_ICUCallBack_ISR(TIM_HandleTypeDef *htim);
void PROX_OverflowCallBack_ISR(TIM_HandleTypeDef *htim);
/*
 * PROX_OverflowCallBack_ISR replaces the old two-timer watchdog.
 * Call it from HAL_TIM_PeriodElapsedCallback when htim->Instance == PROX_TIMER_NO.
 * It counts overflows since the last ICU pulse and resets velocity to zero
 * once VELOCITY_RESET_OVERFLOW_THRESHOLD overflows have elapsed with no capture.
 */


#endif
