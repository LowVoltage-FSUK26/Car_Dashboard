#ifndef PROX_CONFIG_H
#define PROX_CONFIG_H


#define         VELOCITY_RESET_OVERFLOW_THRESHOLD   2


#define         PROX_VELOCITY_RESET_HANDLE  	htim3
#define         PROX_TIMER_HANDLE           	htim3

#define         PROX_VELOCITY_RESET_NO		  	TIM3
#define         PROX_TIMER_NO		           	TIM3


#define         CIRCUM                      	2*18*0.0254*3.14
#define 		TIMER_CLOCK_FREQ 				1000000.0f  // From your 72MHz clock / (71+1) prescaler
#define 		TIMER_PERIOD     				65536


#endif
