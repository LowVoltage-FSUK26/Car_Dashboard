#ifndef DASHBOARD_CONFIG_H
#define DASHBOARD_CONFIG_H



#define         DASHBOARD_BUTTONS_DEBOUNCE_TIMER_HANDLE       htim3
#define         DASHBOARD_LABSTOPWATCH_TIMER_HANDLE           htim4


#define         DASHBOARD_BUTTONS_DEBOUNCE_TIMER_NO 	      TIM3
#define         DASHBOARD_LABSTOPWATCH_TIMER_NO		          TIM4


//ALL NEED TO BE CHANGED//
#define			EXTI_COUNT_STOP_BUTTON			GPIO_PIN_0
#define			EXTI_COUNT_STOP_PIN_IRQ			EXTI0_IRQn
//                                                  ^
//							  change this number according to your choosen GPIO_PIN of (EXTI_COUNT_STOP_BUTTON)

#define			EXTI_CONTINUE_PAUSE_BUTTON		GPIO_PIN_1
#define			EXTI_CONTINUE_PAUSE_IRQ			EXTI1_IRQn
//                                                  ^
//							  change this number according to your choosen GPIO_PIN of (EXTI_CONTINUE_PAUSE_BUTTON)

//***********************************************************FIX BUTTON PINS**********************************************************************//
#define			EXTI_NEXT_PAGE_BUTTON		GPIO_PIN_13
#define			EXTI_NEXT_PAGE_IRQ			EXTI15_10_IRQn

#define			EXTI_PREV_PAGE_BUTTON		GPIO_PIN_14
#define			EXTI_PREV_PAGE_IRQ			EXTI15_10_IRQn

#define			EXTI_REGEN_BUTTON		GPIO_PIN_8 //A8
#define			EXTI_REGEN_IRQ			EXTI8_IRQn

#define			EXTI_DAQ_RESET_BUTTON		GPIO_PIN
#define			EXTI_DAQ_RESET_IRQ			EXTI15_10_IRQn
//**************************************************************************************************************************************************//
//#define			MaxMotor_RPM					6500
//#define			Num_Motor_Poles					4
//#define			ERPM_MaxScale					MaxMotor_RPM * Num_Motor_Poles
#define			ERPM_MaxScale					4294967295 // testing

#define			MaxTemp							65535

#define			MaxPos							255

#define     NEXTION_UART_HANDLE                 huart1

#endif
