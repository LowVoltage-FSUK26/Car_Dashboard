#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"

#include "main.h"

#include "PROX_int.h"
#include "PROX_config.h"

#include "Nextion_int.h"
#include "Nextion_config.h"

extern uint8_t Current_Page;
//Includes---------------------------------------------

extern TIM_HandleTypeDef  PROX_TIMER_HANDLE;
extern UART_HandleTypeDef NEXTION_UART_HANDLE;

extern uint32_t Global_u32Variable_Overflow_Counter;

Prox_Wheel_Velcoity Wheel_Velocity[4] = {0};

/*
 * Snapshot of the global overflow counter at the last ICU capture.
 * PROX_OverflowCallBack_ISR measures elapsed overflows relative to this.
 */
static int32_t Last_ICU_Overflow_Count = 0;

//Nextion
Nextion PROX;


void PROX_INIT()
{
	Nextion_AddComp(&PROX, "Speed", 6, "Racer_Mode");

	HAL_TIM_Base_Start_IT(&PROX_TIMER_HANDLE);
	// Starts TIM3 base interrupt for the global overflow counter + software watchdog

	HAL_TIM_IC_Start_IT(&PROX_TIMER_HANDLE, TIM_CHANNEL_1);  // PB4
	HAL_TIM_IC_Start_IT(&PROX_TIMER_HANDLE, TIM_CHANNEL_2);  // PB5
	// Velocity of the front wheels

	// HAL_TIM_IC_Start_IT(&PROX_TIMER_HANDLE, TIM_CHANNEL_3);
	// HAL_TIM_IC_Start_IT(&PROX_TIMER_HANDLE, TIM_CHANNEL_4);
	// Velocity of the rear wheels (reserved)
}


uint8_t velocity = 0;

/*
 * Called from HAL_TIM_PeriodElapsedCallback when htim->Instance == PROX_TIMER_NO.
 *
 * Software watchdog: counts overflows since the last ICU pulse. If the count
 * reaches VELOCITY_RESET_OVERFLOW_THRESHOLD the sensor has gone quiet, so
 * velocity is forced to zero.
 */
void PROX_OverflowCallBack_ISR(TIM_HandleTypeDef *htim)
{
	if (htim->Instance != PROX_TIMER_NO)
		return;

	int32_t overflows_since_last_pulse = Global_u32Variable_Overflow_Counter - Last_ICU_Overflow_Count;

	if (overflows_since_last_pulse >= VELOCITY_RESET_OVERFLOW_THRESHOLD)
	{
		Nextion_SetVal(&NEXTION_UART_HANDLE, &PROX, 0);
		velocity = 0;

		Wheel_Velocity[FRONT_RIGHT].velocity = 0;
		Wheel_Velocity[FRONT_LEFT].velocity  = 0;

		// Clamp so the condition doesn't fire every overflow after the car stops
		Last_ICU_Overflow_Count = Global_u32Variable_Overflow_Counter;
	}
}


void PROX_ICUCallBack_ISR(TIM_HandleTypeDef *htim)
{
	// Refresh the software watchdog on every capture
	Last_ICU_Overflow_Count = Global_u32Variable_Overflow_Counter;

	/*
     The ICU captures the timer value on each sensor pulse. Because the timer
     runs fast (1 MHz), it may overflow multiple times between two captures.
     We track overflows globally and take a per-channel snapshot to compute:

         Total ticks = (overflow count x TIMER_PERIOD) + captured counter value
	 */
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		Wheel_Velocity[FRONT_RIGHT].curr = htim->Instance->CCR1;

		Wheel_Velocity[FRONT_RIGHT].Actual_Overflow_Count =
				Global_u32Variable_Overflow_Counter - Wheel_Velocity[FRONT_RIGHT].Acculmulated_Overflow_Count;

		Wheel_Velocity[FRONT_RIGHT].diff = Wheel_Velocity[FRONT_RIGHT].curr - Wheel_Velocity[FRONT_RIGHT].prev;
		Wheel_Velocity[FRONT_RIGHT].time =
				(Wheel_Velocity[FRONT_RIGHT].diff + (Wheel_Velocity[FRONT_RIGHT].Actual_Overflow_Count * TIMER_PERIOD))
				/ TIMER_CLOCK_FREQ;

		Wheel_Velocity[FRONT_RIGHT].prev = Wheel_Velocity[FRONT_RIGHT].curr;
		Wheel_Velocity[FRONT_RIGHT].Acculmulated_Overflow_Count = Global_u32Variable_Overflow_Counter;
	}

	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	{
		Wheel_Velocity[FRONT_LEFT].curr = htim->Instance->CCR2;

		Wheel_Velocity[FRONT_LEFT].Actual_Overflow_Count =
				Global_u32Variable_Overflow_Counter - Wheel_Velocity[FRONT_LEFT].Acculmulated_Overflow_Count;

		Wheel_Velocity[FRONT_LEFT].diff = Wheel_Velocity[FRONT_LEFT].curr - Wheel_Velocity[FRONT_LEFT].prev;
		Wheel_Velocity[FRONT_LEFT].time =
				(Wheel_Velocity[FRONT_LEFT].diff + (Wheel_Velocity[FRONT_LEFT].Actual_Overflow_Count * TIMER_PERIOD))
				/ TIMER_CLOCK_FREQ;

		Wheel_Velocity[FRONT_LEFT].prev = Wheel_Velocity[FRONT_LEFT].curr;
		Wheel_Velocity[FRONT_LEFT].Acculmulated_Overflow_Count = Global_u32Variable_Overflow_Counter;
	}

	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
	{
		Wheel_Velocity[REAR_LEFT].curr = htim->Instance->CCR3;
	}

	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
	{
		Wheel_Velocity[REAR_RIGHT].curr = htim->Instance->CCR4;
	}


	if (Wheel_Velocity[FRONT_RIGHT].time > 0)
	{
		Wheel_Velocity[FRONT_RIGHT].velocity =
				((float)CIRCUM / (Wheel_Velocity[FRONT_RIGHT].time * 4)) * 3.6f;
		//                                                              ^ m/s to km/h
	}

	if (Wheel_Velocity[FRONT_LEFT].time > 0)
	{
		Wheel_Velocity[FRONT_LEFT].velocity =
				((float)CIRCUM / (Wheel_Velocity[FRONT_LEFT].time * 4)) * 3.6f;
		//                                                             ^ m/s to km/h
	}

	velocity = (Wheel_Velocity[FRONT_RIGHT].velocity + Wheel_Velocity[FRONT_LEFT].velocity) / 2;
	// Front wheels only — true speed reference for the car

	Nextion_SetVal(&NEXTION_UART_HANDLE, &PROX, velocity);
}
