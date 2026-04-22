#ifndef DASHBOARD_CONFIG_H
#define DASHBOARD_CONFIG_H


#define         DASHBOARD_BUTTONS_DEBOUNCE_TIMER_HANDLE       htim1
#define         DASHBOARD_LABSTOPWATCH_TIMER_HANDLE           htim4

#define         DASHBOARD_BUTTONS_DEBOUNCE_TIMER_NO           TIM1
#define         DASHBOARD_LABSTOPWATCH_TIMER_NO               TIM4

#define         EXTI_LAP_RESET_BUTTON       GPIO_PIN_0    // PA0
#define         EXTI_LAP_RESET_IRQ          EXTI0_IRQn

#define         EXTI_COUNT_STOP_BUTTON      GPIO_PIN_1    // PA1 - Start/Stop
#define         EXTI_COUNT_STOP_IRQ         EXTI1_IRQn

#define         EXTI_DAQ_RESET_BUTTON       GPIO_PIN_8    // PA8
#define         EXTI_DAQ_RESET_IRQ          EXTI9_5_IRQn  // covers pins 5-9

#define         EXTI_PREV_PAGE_BUTTON       GPIO_PIN_13   // PB13
#define         EXTI_PREV_PAGE_IRQ          EXTI15_10_IRQn

#define         EXTI_NEXT_PAGE_BUTTON       GPIO_PIN_14   // PC14
#define         EXTI_NEXT_PAGE_IRQ          EXTI15_10_IRQn

#define         EXTI_REGEN_BUTTON           GPIO_PIN_15   // PB15 - Regen Braking
#define         EXTI_REGEN_IRQ              EXTI15_10_IRQn

//#define			MaxMotor_RPM					6500
//#define			Num_Motor_Poles					4
//#define			ERPM_MaxScale					MaxMotor_RPM * Num_Motor_Poles

#define			ERPM_MaxScale					4294967295 // testing

#define			MaxTemp							65535

#define			MaxPos							255

#define     NEXTION_UART_HANDLE                 huart1

#define CAN_VOLT_ID         0x100   // Match BMS master's CAN_VOLT_ID
#define CAN_TEMP_ID         0x200   // Match BMS master's CAN_TEMP_ID
#define CAN_DATA_PER_FRAME  4       // 4 slaves per frame (8 bytes / 2 bytes each)
#endif
