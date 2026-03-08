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
extern TIM_HandleTypeDef  PROX_VELOCITY_RESET_HANDLE;
extern UART_HandleTypeDef NEXTION_UART_HANDLE;

extern uint32_t Global_u32Variable_Overflow_Counter;

Prox_Wheel_Velcoity Wheel_Velocity[4] = {0};


//Nextion
Nextion PROX;


void PROX_INIT()
{
    Nextion_AddComp(&PROX, "Speed", 6, "Racer_Mode");

    HAL_TIM_Base_Start_IT(&PROX_TIMER_HANDLE);
    // For the global overflow counter

    HAL_TIM_IC_Start_IT(&PROX_TIMER_HANDLE, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&PROX_TIMER_HANDLE, TIM_CHANNEL_2);
    // Velocity of the Front wheels

    // HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3);
    // HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_4);
    // Velocity of the Rear wheels

}


float time_Average = 0;
uint8_t velocity = 0;
void PROX_ResetVelocity(TIM_HandleTypeDef *htim)
{
	Nextion_SetVal(&NEXTION_UART_HANDLE, &PROX, 0);
	velocity = 0;

	HAL_TIM_Base_Stop_IT(&PROX_VELOCITY_RESET_HANDLE);
	// The interrupt of this timer is to ensure that the velocity will be equal zero if no ICU interrupts have been triggered within 100msec.

	__HAL_TIM_CLEAR_IT(&PROX_VELOCITY_RESET_HANDLE, TIM_IT_UPDATE);
}


void PROX_ICUCallBack_ISR(TIM_HandleTypeDef *htim)
{
	__HAL_TIM_SET_COUNTER(&PROX_VELOCITY_RESET_HANDLE, 0);
	HAL_TIM_Base_Start_IT(&PROX_VELOCITY_RESET_HANDLE);
	/* Reseting the timer that sets the velocity into zero. (Think about it like the watchdog timer).*/
	


	/*
	 The ICU (Input Capture Unit) is used to capture the timer value and calculate the difference between the current and the previous capture.
	 To achieve higher resolution, we increased the timer clock speed. However, this caused a problem: the timer may overflow multiple times
	 between two captures.

	 To solve this, we count the number of overflows that occur between two captures. Each overflow corresponds to the maximum counter
	 value of the timer. Therefore, the total elapsed ticks can be calculated as:

	 Total ticks = (overflow count × maximum counter value) + current captured counter value.

	 Since the overflow counter is global, we need to create a local overflow counter for each ICU channel.
	 This way, each channel tracks its own overflows independently, while still using the global overflow counter as a reference.
	 */
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{	
		Wheel_Velocity[FRONT_RIGHT].curr = htim->Instance->CCR1;
		
		Wheel_Velocity[FRONT_RIGHT].Actual_Overflow_Count = Global_u32Variable_Overflow_Counter - Wheel_Velocity[FRONT_RIGHT].Acculmulated_Overflow_Count;
//		Wheel_Velocity[FRONT_RIGHT].time = ((int32_t)Wheel_Velocity[FRONT_RIGHT].curr - (int32_t)Wheel_Velocity[FRONT_RIGHT].prev + Wheel_Velocity[FRONT_RIGHT].Actual_Overflow_Count*65535) / 1000000.0;
																																										//Timer Prescaler

		Wheel_Velocity[FRONT_RIGHT].diff = Wheel_Velocity[FRONT_RIGHT].curr - Wheel_Velocity[FRONT_RIGHT].prev;
		Wheel_Velocity[FRONT_RIGHT].time = (Wheel_Velocity[FRONT_RIGHT].diff + (Wheel_Velocity[FRONT_RIGHT].Actual_Overflow_Count * TIMER_PERIOD)) / TIMER_CLOCK_FREQ;


		Wheel_Velocity[FRONT_RIGHT].prev = Wheel_Velocity[FRONT_RIGHT].curr;
		Wheel_Velocity[FRONT_RIGHT].Acculmulated_Overflow_Count = Global_u32Variable_Overflow_Counter;
	}
	
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	{
		Wheel_Velocity[FRONT_LEFT].curr = htim->Instance->CCR2;
		
		Wheel_Velocity[FRONT_LEFT].Actual_Overflow_Count = Global_u32Variable_Overflow_Counter - Wheel_Velocity[FRONT_LEFT].Acculmulated_Overflow_Count;
//		Wheel_Velocity[FRONT_LEFT].time = ((int32_t)Wheel_Velocity[FRONT_LEFT].curr - (int32_t)Wheel_Velocity[FRONT_LEFT].prev + Wheel_Velocity[FRONT_LEFT].Actual_Overflow_Count*65535) / 1000000.0;
																																									//Timer Prescaler

		Wheel_Velocity[FRONT_LEFT].diff = Wheel_Velocity[FRONT_LEFT].curr - Wheel_Velocity[FRONT_LEFT].prev;
		Wheel_Velocity[FRONT_LEFT].time = (Wheel_Velocity[FRONT_LEFT].diff + (Wheel_Velocity[FRONT_LEFT].Actual_Overflow_Count * TIMER_PERIOD)) / TIMER_CLOCK_FREQ;

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





	if(Wheel_Velocity[FRONT_RIGHT].time > 0)
	{
		Wheel_Velocity[FRONT_RIGHT].velocity = ((float)CIRCUM / (Wheel_Velocity[FRONT_RIGHT].time * 4)) * 3.6f;
																							//             ^
																							// converts m/sec to Km/hr
	}
	if(Wheel_Velocity[FRONT_LEFT].time > 0)
	{
		Wheel_Velocity[FRONT_LEFT].velocity = ((float)CIRCUM / (Wheel_Velocity[FRONT_LEFT].time * 4)) * 3.6f;
																						  //             ^
																						  // converts m/sec to Km/hr
	}

		velocity  = (Wheel_Velocity[FRONT_RIGHT].velocity + Wheel_Velocity[FRONT_LEFT].velocity) / 2;
		//We only takes the velocity of the front wheels as they are the real indication for the speed of the car.

    	Nextion_SetVal(&NEXTION_UART_HANDLE, &PROX, velocity);

}
