#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"

#include "main.h"

#include "Dashboard_int.h"
#include "Dashboard_config.h"

#include "Nextion_int.h"
#include "Nextion_config.h"
// #includes ----------------------------

extern UART_HandleTypeDef NEXTION_UART_HANDLE;
extern TIM_HandleTypeDef  DASHBOARD_LABSTOPWATCH_TIMER_HANDLE; // For the Lap StopWatch
extern TIM_HandleTypeDef  DASHBOARD_BUTTONS_DEBOUNCE_TIMER_HANDLE;

extern CAN_HandleTypeDef  hcan;
extern uint8_t Current_Page;

Nextion Lap_Time;
Nextion Last_Lap;
Nextion Best_Lap;

Nextion ERPM_Slider;
Nextion Temp_Slider;
Nextion Throttle_Slider;
Nextion Brake_Slider;
Nextion Battery_Perc;

Nextion RF_Temp;
Nextion LF_Temp;
Nextion RR_Temp;
Nextion LR_Temp;

Nextion RF_Travel;
Nextion LF_Travel;
Nextion RR_Travel;
Nextion LR_Travel;
Nextion Batt_Temp;

//New warning pics
Nextion Battery_Temp_Warning;
Nextion Motor_Temp_Warning;
Nextion	Error_Warning;

Nextion T[32];
Nextion V[32];



void Dashboard_Init(void)
{
	Nextion_AddComp(&Lap_Time, "Lap_Time", 17, "Racer_Mode");
	Nextion_AddComp(&Last_Lap, "Last_Lap", 18, "Racer_Mode");
	Nextion_AddComp(&Best_Lap, "Best_Lap", 19, "Racer_Mode");


	Nextion_AddComp(&ERPM_Slider, "RPM_Slider",  1, "Racer_Mode");
	Nextion_AddComp(&Temp_Slider, "Temp_Slider",  5, "Racer_Mode");
	Nextion_AddComp(&Throttle_Slider, "Thrott_Slider",  23, "Racer_Mode");
	Nextion_AddComp(&Brake_Slider, "Brake_Slider",  24, "Racer_Mode");
	Nextion_AddComp(&Battery_Perc, "Battery_Perc",  20, "Racer_Mode");
	//*******************************************************************NEW CODE****************************************************************//
	Nextion_AddComp(&Batt_Temp, "Batt_Temp",  28, "Racer_Mode");

	//Warning pics
	Nextion_AddComp(&Battery_Temp_Warning, "Battery_Warn",25, "Racer_Mode"); //set vis to 0 in init
	Nextion_AddComp(&Motor_Temp_Warning, "Motor_Warn",26, "Racer_Mode");  //set vis to 0
	Nextion_AddComp(&Error_Warning, "Error_Warning",27, "Racer_Mode"); //set vis to 0
	//Diagnostic page Comps
	//	Nextion_AddComp(&Temp_Slider, "Temp_Slider",  7, "Diagnostics");
	//	Nextion_AddComp(&Throttle_Slider, "Thrott_Slider",  2, "Diagnostics");
	//	Nextion_AddComp(&Brake_Slider, "Brake_Slider",  3, "Diagnostics");
	//	Nextion_AddComp(&Battery_Perc, "Battery_Perc",  9, "Diagnostics");

	Nextion_AddComp(&RF_Temp, "RF_Temp",  21, "Diagnostics");
	Nextion_AddComp(&LF_Temp, "LF_Temp",  17, "Diagnostics");
	Nextion_AddComp(&RR_Temp, "RR_Temp",  23, "Diagnostics");
	Nextion_AddComp(&LR_Temp, "LR_Temp",  19, "Diagnostics");

	//Nextion_AddComp(&Batt_Temp, "Batt_Temp",  11, "Diagnostics");

	Nextion_Set_Vis(&huart1, &Battery_Temp_Warning,NEXTION_VIS_OFF);
	Nextion_Set_Vis(&huart1, &Motor_Temp_Warning,NEXTION_VIS_OFF);
	Nextion_Set_Vis(&huart1, &Error_Warning,NEXTION_VIS_OFF);

	//	Nextion_AddComp(&Temp_Slider, "Temp_Slider",  7, "Diagnostics2");
	//	Nextion_AddComp(&Throttle_Slider, "Thrott_Slider",  2, "Diagnostics2");
	//	Nextion_AddComp(&Brake_Slider, "Brake_Slider",  3, "Diagnostics2");
	//	Nextion_AddComp(&Battery_Perc, "Battery_Perc",  9, "Diagnostics2");

	Nextion_AddComp(&RF_Travel, "RF_Travel",  15, "Diagnostics2");
	Nextion_AddComp(&LF_Travel, "LF_Travel",  13, "Diagnostics2");
	Nextion_AddComp(&RR_Travel, "RR_Travel",  16, "Diagnostics2");
	Nextion_AddComp(&LR_Travel, "LR_Travel",  14, "Diagnostics2");

	Nextion_CreateDiagnostics(T,V);

	//Nextion_AddComp(&Batt_Temp, "Batt_Temp",  11, "Diagnostics2");



	CAN_FilterTypeDef canFilterConfig;
	canFilterConfig.FilterBank = 0;
	canFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	canFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	canFilterConfig.FilterIdHigh = 0x0000;
	canFilterConfig.FilterIdLow = 0x0000;
	canFilterConfig.FilterMaskIdHigh = 0x0000;
	canFilterConfig.FilterMaskIdLow = 0x0000;
	canFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	canFilterConfig.FilterActivation = ENABLE;
	canFilterConfig.SlaveStartFilterBank = 14;
	if(HAL_CAN_ConfigFilter(&hcan, &canFilterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

	//Page no #96
	if (HAL_CAN_Start(&hcan) != HAL_OK)
	{
		Error_Handler();
	}

}



//=============================================================================================================================================================
//===============================================Start: Lap Timer==============================================================================================
//=============================================================================================================================================================




/*RTOS Task that will be executed every 10msec to increase the (Lap Timer) on the dashboard with 0.01sec

-->>Note<<-- 
    This function will be suspended directly after creation and will start executing once a GPIO_PIN is hit
 */
StopWatch Lap_Timer = {0};
char Global_u8Array_Tx_Buffer[30] = {0};
void LabTimer_CallBack_ISR()
{
	Lap_Timer.mili_Sec += 1;

	Lap_Timer.Sec_dec_1 = Lap_Timer.mili_Sec % 10;
	Lap_Timer.Sec_dec_2 = (Lap_Timer.mili_Sec % 100) / 10;

	Lap_Timer.Seconds = Lap_Timer.mili_Sec / 100 ;
	Lap_Timer.Sec_dig_1 = Lap_Timer.Seconds % 10;
	Lap_Timer.Sec_dig_2 = (Lap_Timer.Seconds / 10) % 6;

	Lap_Timer.Minutes = Lap_Timer.Seconds / 60;
	Lap_Timer.Min_d1 = Lap_Timer.Minutes % 10;
	Lap_Timer.Min_d2 = Lap_Timer.Minutes / 10;



	sprintf(Global_u8Array_Tx_Buffer, "%u%u:%u%u.%u%u", Lap_Timer.Min_d2, Lap_Timer.Min_d1, Lap_Timer.Sec_dig_2, Lap_Timer.Sec_dig_1, Lap_Timer.Sec_dec_2, Lap_Timer.Sec_dec_1);

	Nextion_SetText(&NEXTION_UART_HANDLE, &Lap_Time, Global_u8Array_Tx_Buffer);
}
//---------------------------------------------------------------------------------------------------------------------




char Global_u8Array_Tx_BestLap_Buffer[30] = {0};
uint8_t  BestLap_Flag = 0;
uint32_t BestLap_Centi_Sec = 0;
void LabTimerStoped()
{
	sprintf(Global_u8Array_Tx_Buffer, "%u%u:%u%u.%u%u", Lap_Timer.Min_d2, Lap_Timer.Min_d1, Lap_Timer.Sec_dig_2, Lap_Timer.Sec_dig_1, Lap_Timer.Sec_dec_2, Lap_Timer.Sec_dec_1);

	if((BestLap_Flag == 0) || (BestLap_Centi_Sec > Lap_Timer.mili_Sec))
	{
		BestLap_Centi_Sec = Lap_Timer.mili_Sec;
		sprintf(Global_u8Array_Tx_BestLap_Buffer, "%u%u:%u%u.%u%u", Lap_Timer.Min_d2, Lap_Timer.Min_d1, Lap_Timer.Sec_dig_2, Lap_Timer.Sec_dig_1, Lap_Timer.Sec_dec_2, Lap_Timer.Sec_dec_1);
		Nextion_SetText(&NEXTION_UART_HANDLE, &Best_Lap, Global_u8Array_Tx_BestLap_Buffer);

		BestLap_Flag = 1;
	}
	Nextion_SetText(&NEXTION_UART_HANDLE, &Lap_Time, "00:00.00");
	Nextion_SetText(&NEXTION_UART_HANDLE, &Last_Lap, Global_u8Array_Tx_Buffer);
}




uint8_t StopWatch_State = STOPWATCH_STOPED;
void LabTimer_Buttons_Callback(TIM_HandleTypeDef *htim, uint16_t GPIO_Pin)
{
	HAL_NVIC_DisableIRQ(EXTI_COUNT_STOP_IRQ);
	HAL_NVIC_DisableIRQ(EXTI_LAP_RESET_IRQ);

	if((GPIO_Pin == EXTI_COUNT_STOP_BUTTON) && (StopWatch_State == STOPWATCH_STOPED))
	{
		//-------------------------------------------------------------------

		Lap_Timer.mili_Sec = 0; Lap_Timer.Seconds = 0; Lap_Timer.Minutes = 0;

		StopWatch_State = STOPWATCH_COUNTING;
		HAL_TIM_Base_Start_IT(&DASHBOARD_LABSTOPWATCH_TIMER_HANDLE);

		//-------------------------------------------------------------------
		HAL_TIM_Base_Start_IT(&DASHBOARD_BUTTONS_DEBOUNCE_TIMER_HANDLE);
	}
	else if((GPIO_Pin == EXTI_COUNT_STOP_BUTTON) && ((StopWatch_State == STOPWATCH_COUNTING) || (StopWatch_State == STOPWATCH_PAUSED)))
	{
		//-------------------------------------------------------------------

		StopWatch_State = STOPWATCH_STOPED;
		HAL_TIM_Base_Stop_IT(&DASHBOARD_LABSTOPWATCH_TIMER_HANDLE);
		LabTimerStoped();

		//-------------------------------------------------------------------
		HAL_TIM_Base_Start_IT(&DASHBOARD_BUTTONS_DEBOUNCE_TIMER_HANDLE);
	}
	//	else if((GPIO_Pin == EXTI_LAP_RESET_BUTTON) && (StopWatch_State != STOPWATCH_STOPED))
	//	{
	//		// Reset: stop the timer, clear the count, reset display
	//		//-------------------------------------------------------------------
	//
	//		StopWatch_State = STOPWATCH_STOPED;
	//		HAL_TIM_Base_Stop_IT(&DASHBOARD_LABSTOPWATCH_TIMER_HANDLE);
	//
	//		Lap_Timer.mili_Sec = 0; Lap_Timer.Seconds = 0; Lap_Timer.Minutes = 0;
	//		Lap_Timer.Sec_dec_1 = 0; Lap_Timer.Sec_dec_2 = 0;
	//		Lap_Timer.Sec_dig_1 = 0; Lap_Timer.Sec_dig_2 = 0;
	//		Lap_Timer.Min_d1 = 0; Lap_Timer.Min_d2 = 0;
	//
	//		Nextion_SetText(&NEXTION_UART_HANDLE, &Lap_Time, "00:00.00");
	//
	//		//-------------------------------------------------------------------
	//		HAL_TIM_Base_Start_IT(&DASHBOARD_BUTTONS_DEBOUNCE_TIMER_HANDLE);
	//	}
	else if((GPIO_Pin == EXTI_LAP_RESET_BUTTON) && (StopWatch_State == STOPWATCH_STOPED))
	{
		// Reset Best Lap
		BestLap_Flag = 0;
		BestLap_Centi_Sec = 0;
		memset(Global_u8Array_Tx_BestLap_Buffer, 0, sizeof(Global_u8Array_Tx_BestLap_Buffer));

		// Reset Prev Lap (Last Lap)
		memset(Global_u8Array_Tx_Buffer, 0, sizeof(Global_u8Array_Tx_Buffer));

		// Reset all Lap_Timer fields
		memset(&Lap_Timer, 0, sizeof(Lap_Timer));

		// Reset Nextion displays
		Nextion_SetText(&NEXTION_UART_HANDLE, &Lap_Time,  "00:00.00");
		Nextion_SetText(&NEXTION_UART_HANDLE, &Last_Lap,  "00:00.00");
		Nextion_SetText(&NEXTION_UART_HANDLE, &Best_Lap,  "00:00.00");

		HAL_TIM_Base_Start_IT(&DASHBOARD_BUTTONS_DEBOUNCE_TIMER_HANDLE);
	}
}



void Debouncing_CallBack(TIM_HandleTypeDef *htim)
{
	HAL_TIM_Base_Stop_IT(&DASHBOARD_BUTTONS_DEBOUNCE_TIMER_HANDLE);

	// Clear pending flags for all buttons
	__HAL_GPIO_EXTI_CLEAR_FLAG(EXTI_LAP_RESET_BUTTON);
	__HAL_GPIO_EXTI_CLEAR_FLAG(EXTI_COUNT_STOP_BUTTON);
	__HAL_GPIO_EXTI_CLEAR_FLAG(EXTI_DAQ_RESET_BUTTON);
	__HAL_GPIO_EXTI_CLEAR_FLAG(EXTI_PREV_PAGE_BUTTON);
	__HAL_GPIO_EXTI_CLEAR_FLAG(EXTI_NEXT_PAGE_BUTTON);
	__HAL_GPIO_EXTI_CLEAR_FLAG(EXTI_REGEN_BUTTON);

	// Re-enable IRQs
	HAL_NVIC_EnableIRQ(EXTI_LAP_RESET_IRQ);
	HAL_NVIC_EnableIRQ(EXTI_COUNT_STOP_IRQ);
	HAL_NVIC_EnableIRQ(EXTI_DAQ_RESET_IRQ);
	HAL_NVIC_EnableIRQ(EXTI_PREV_PAGE_IRQ);
	HAL_NVIC_EnableIRQ(EXTI_NEXT_PAGE_IRQ);
	HAL_NVIC_EnableIRQ(EXTI_REGEN_IRQ);
}
//=============================================================================================================================================================
//===============================================End: Lap Timer================================================================================================
//=============================================================================================================================================================




//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------




//=============================================================================================================================================================
//===============================================Start: Parsing CAN Message====================================================================================
//=============================================================================================================================================================


CAN_RxHeaderTypeDef CANRxHeader;
uint8_t Received_CAN_Message[8] = {0};
uint32_t ERPM_Val = 0;
uint16_t MotorTemp_Val = 0;
uint8_t Thrott_Pos = 0;
uint8_t Brake_Pos = 0;
uint16_t Battery = 0;
uint8_t  Batt_int = 0;
uint8_t  Batt_Deci = 0;
uint16_t BatteryTemp_Val = 0;
//new
uint8_t RF_temp_val= 0;
uint8_t LF_temp_val= 0;
uint8_t RR_temp_val= 0;
uint8_t LR_temp_val= 0;

uint8_t RF_trav_val= 0;
uint8_t LF_trav_val= 0;
uint8_t RR_trav_val= 0;
uint8_t LR_trav_val= 0;

GPIO_TypeDef* LED_PORT[9] = {
		GPIOB, GPIOB, GPIOB, GPIOB, GPIOB,
		GPIOA, GPIOA, GPIOA, GPIOA
};

uint16_t LED_PIN[9] = {
		GPIO_PIN_11,
		GPIO_PIN_10,
		GPIO_PIN_2,
		GPIO_PIN_1,
		GPIO_PIN_0,
		GPIO_PIN_7,
		GPIO_PIN_6,
		GPIO_PIN_5,
		GPIO_PIN_4
};
char test_Buffer[30] = {0};

BMS_CAN_Frame_t RxData;

void CAN_Message(CAN_HandleTypeDef *hcan)
{
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CANRxHeader, RxData.bytes);
	uint32_t id = CANRxHeader.StdId;
	//---------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------START OF BMS TEST------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------

	//========================================================
	    // BMS Slave Voltage Frames: CAN_VOLT_ID+0 to CAN_VOLT_ID+7
	    // Union decode: slave[4] x uint16_t, little-endian (STM32 native)
	    // Display format: raw / 100 = integer volts  e.g. 372 -> "3 v"
	    //========================================================

	//========================================================
	    // BMS Slave Voltage Frames
	    //========================================================
	    if (id >= CAN_VOLT_ID && id < (CAN_VOLT_ID + (32 / CAN_DATA_PER_FRAME)))
	    {
	        uint8_t first_slave = ((id - CAN_VOLT_ID) * 4) + 1;
//	        uint8_t first_slave = frame_index * CAN_DATA_PER_FRAME;

//	        memcpy(RxData.bytes, Received_CAN_Message, 8);

	        for (uint8_t i = 0; i < CAN_DATA_PER_FRAME; i++)
	        {
	            uint8_t slave_idx = first_slave + i;

	            sprintf(test_Buffer, "%d v", (RxData.frame.slave[i] / 100));
	            Nextion_SetText(&NEXTION_UART_HANDLE, &V[slave_idx - 1], test_Buffer);
	        }
	        return;
	    }

	    //========================================================
	    // BMS Slave Temperature Frames
	    //========================================================
	    if (id >= CAN_TEMP_ID && id < (CAN_TEMP_ID + (32 / CAN_DATA_PER_FRAME)))
	    {
	        uint8_t frame_index = id - CAN_TEMP_ID;
	        uint8_t first_slave = frame_index * CAN_DATA_PER_FRAME;

	        memcpy(RxData.bytes, Received_CAN_Message, 8);

	        for (uint8_t i = 0; i < CAN_DATA_PER_FRAME; i++)
	        {
	            uint8_t slave_idx = first_slave + i;

	            sprintf(test_Buffer, "%d C", (RxData.frame.slave[i] / 100));
	            Nextion_SetText(&NEXTION_UART_HANDLE, &T[slave_idx], test_Buffer);
	        }
	        return;
	    }
//	uint32_t id = CANRxHeader.StdId;
//
//	//========================================================
//	// BMS Slave Voltage Frames: CAN_VOLT_ID+0 to CAN_VOLT_ID+7
//	// Each frame carries 4 slaves, 2 bytes each (big-endian)
//	//========================================================
//	if (id >= CAN_VOLT_ID && id < (CAN_VOLT_ID + (32 / CAN_DATA_PER_FRAME)))
//	{
//		uint8_t frame_index = id - CAN_VOLT_ID;
//		uint8_t first_slave = frame_index * CAN_DATA_PER_FRAME;
//
//		for (uint8_t i = 0; i < CAN_DATA_PER_FRAME; i++)
//		{
//			uint8_t  slave_idx = first_slave + i;
//			uint16_t raw_val   = ((uint16_t)Received_CAN_Message[i * 2] << 8)
//	                                		 | Received_CAN_Message[i * 2 + 1];
//
//			uint8_t volt_int  = raw_val / 100;
//			uint8_t volt_deci = raw_val % 100;
//
//			sprintf(test_Buffer, "%u.%02u", volt_int, volt_deci);
//			Nextion_SetText(&NEXTION_UART_HANDLE, &V[slave_idx], test_Buffer);
//		}
//		return;
//	}
//
//	//========================================================
//	// BMS Slave Temperature Frames: CAN_TEMP_ID+0 to CAN_TEMP_ID+7
//	// Each frame carries 4 slaves, 2 bytes each (big-endian)
//	//========================================================
//	if (id >= CAN_TEMP_ID && id < (CAN_TEMP_ID + (32 / CAN_DATA_PER_FRAME)))
//	{
//		uint8_t frame_index = id - CAN_TEMP_ID;
//		uint8_t first_slave = frame_index * CAN_DATA_PER_FRAME;
//
//		for (uint8_t i = 0; i < CAN_DATA_PER_FRAME; i++)
//		{
//			uint8_t  slave_idx = first_slave + i;
//			uint16_t raw_val   = ((uint16_t)Received_CAN_Message[i * 2] << 8)
//	                                		 | Received_CAN_Message[i * 2 + 1];
//
//			uint8_t temp_int  = raw_val / 10;
//			uint8_t temp_deci = raw_val % 10;
//
//			sprintf(test_Buffer, "%u.%u", temp_int, temp_deci);
//			Nextion_SetText(&NEXTION_UART_HANDLE, &T[slave_idx], test_Buffer);
//		}
//		return;
//	}
	//---------------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------END OF BMS TEST------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------
	switch(CANRxHeader.StdId)
	{
	case 0x20: //ERPM, "From the Motor Controller" //not correct maybe???

		//little endian might need to be changed later
		//ERPM_Val = (Received_CAN_Message[0]) | (Received_CAN_Message[1] << 8) | (Received_CAN_Message[2] << 16) | (Received_CAN_Message[3] << 24);
		ERPM_Val = (Received_CAN_Message[0]<<24) | (Received_CAN_Message[1] << 16) | (Received_CAN_Message[2] << 8) | (Received_CAN_Message[3]);
		ERPM_Val = (float)(ERPM_Val/200.0) * 100.0; // Mapping ERPM value to percentage scale. change 200 to actual max value


		uint8_t led_count = (uint8_t)((ERPM_Val / 100.0f) * 9.0f);

		for(uint8_t i=0;i<9;i++)
		{
			HAL_GPIO_WritePin(LED_PORT[i], LED_PIN[i],
					(i < led_count) ? GPIO_PIN_SET : GPIO_PIN_RESET);
		}

		Nextion_SetVal(&NEXTION_UART_HANDLE, &ERPM_Slider, ERPM_Val);
		break;

	case 0x22: //Motor Temp, "From the Motor Controller"

		//little endian might need to change later and check  where in the message it is
		//MotorTemp_Val = (Received_CAN_Message[0]) | (Received_CAN_Message[1] << 8);

		MotorTemp_Val = (Received_CAN_Message[6] << 8) | Received_CAN_Message[7];
		MotorTemp_Val = (float)(MotorTemp_Val/200.0) * 100.0; // Mapping MotrTemp value to percentage scale.


		if (MotorTemp_Val>=90)
		{
			Nextion_Set_Vis(&huart1, &Motor_Temp_Warning, NEXTION_VIS_ON);
		}
		else
		{
			Nextion_Set_Vis(&huart1, &Motor_Temp_Warning, NEXTION_VIS_OFF);
		}
		Nextion_SetVal(&NEXTION_UART_HANDLE, &Temp_Slider, MotorTemp_Val);
		break;

	case 0x24: //Throttle & Brake Position, "From the Motor Controller" //not from controller needs to change

		Thrott_Pos = Received_CAN_Message[0];
		Thrott_Pos = (float)(Thrott_Pos/200.0) * 100.0;
		Brake_Pos  = Received_CAN_Message[1];
		Brake_Pos  = (float)(Brake_Pos/200.0) * 100.0;

		Nextion_SetVal(&NEXTION_UART_HANDLE, &Throttle_Slider, Thrott_Pos);
		Nextion_SetVal(&NEXTION_UART_HANDLE, &Brake_Slider, Brake_Pos);
		break;

	case 0x50: //Testing from IC ECU "battery level" on CAN

		Battery = (Received_CAN_Message[2] << 8) | Received_CAN_Message[3];

		Batt_int = Battery / 100;
		Batt_Deci = Battery % 100;

		sprintf(test_Buffer, "%u.%u", Batt_int, Batt_Deci);
		Nextion_SetText(&NEXTION_UART_HANDLE, &Battery_Perc, test_Buffer);

		break;
		//************************************************************NEW CODE*********************************************************************//
		//	case 0x55: // CAN message for battery temp
		//		BatteryTemp_Val = (Received_CAN_Message[0]) | (Received_CAN_Message[1] << 8);
		//
		//		//BatteryTemp_Val = (float)(MotorTemp_Val/200.0) * 100.0; // Mapping Battery Temp value to percentage scale.
		//
		//		if (BatteryTemp_Val>=90)
		//		{
		//			Nextion_Set_Vis(&huart1, &Battery_Temp_Warning, NEXTION_VIS_ON);
		//		}
		//		else
		//		{
		//			Nextion_Set_Vis(&huart1, &Battery_Temp_Warning, NEXTION_VIS_OFF);
		//		}
		//		Nextion_SetVal(&NEXTION_UART_HANDLE, &Batt_Temp, BatteryTemp_Val);
		//		break;
		//each in different message (Needs checking)
	case 0x56: // CAN message for RF tire temp
	{
		RF_temp_val = Received_CAN_Message[0];
		Nextion_SetText(&NEXTION_UART_HANDLE, &RF_Temp, RF_temp_val);
		break;
	}
	case 0x57: // CAN message for LF tire temp
	{
		LF_temp_val = Received_CAN_Message[0];
		Nextion_SetText(&NEXTION_UART_HANDLE, &LF_Temp, LF_temp_val);
		break;
	}
	case 0x58: // CAN message for RR tire temp
	{
		RR_temp_val = Received_CAN_Message[0];
		Nextion_SetText(&NEXTION_UART_HANDLE, &RR_Temp, RR_temp_val);
		break;
	}
	case 0x59: // CAN message for LR tire temp
	{
		LR_temp_val = Received_CAN_Message[0];
		Nextion_SetText(&NEXTION_UART_HANDLE, &LR_Temp, LR_temp_val);
		break;
	}

	//one message all travel + brake pressure
	case 0x60: // CAN message for RF travel temp
	{
		if(Current_Page==3)
		{
			//fill in v and T from 1-16
		}
		else if(Current_Page==4)
		{
			//fill in v and T from 17-32
		}
		else
		{
			break;
		}

	}
	case 0x61:
	{

	}




	/* Battery level case:
	 *
	 *
	 *
	 *
	 *
	 *
	 *
	 *
	 */
	}
}
//=============================================================================================================================================================
//===============================================End: Parsing CAN Message======================================================================================
//=============================================================================================================================================================

void Nextion_NextPage()
{
	HAL_NVIC_DisableIRQ(EXTI_NEXT_PAGE_IRQ);
	switch(Current_Page)
	{
	case 1:
		Nextion_SetPage(&huart1,"Diagnostics");
		Current_Page = 2;
		break;
	case 2:
		Nextion_SetPage(&huart1,"Diagnostics2");
		Current_Page = 3;
		break;
	case 3:
		Nextion_SetPage(&huart1,"Diagnostics3");
		Current_Page = 4;
		break;
	case 4:
		Nextion_SetPage(&huart1,"Diagnostics4");
		Current_Page = 5;
		break;
	case 5:
		Nextion_SetPage(&huart1,"Racer_Mode");
		Current_Page = 1;
		break;
	}
	HAL_TIM_Base_Start_IT(&DASHBOARD_BUTTONS_DEBOUNCE_TIMER_HANDLE);
}
void Nextion_PrevPage()
{
	HAL_NVIC_DisableIRQ(EXTI_PREV_PAGE_IRQ);
	switch(Current_Page)
	{
	case 1:
		Nextion_SetPage(&huart1,"Diagnostics4");
		Current_Page = 5;
		break;
	case 2:
		Nextion_SetPage(&huart1,"Racer_Mode");
		Current_Page = 1;
		break;
	case 3:
		Nextion_SetPage(&huart1,"Diagnostics");
		Current_Page = 2;
		break;
	case 4:
		Nextion_SetPage(&huart1,"Diagnostics2");
		Current_Page = 3;
		break;
	case 5:
		Nextion_SetPage(&huart1,"Diagnostics3");
		Current_Page = 4;
		break;
	}
	HAL_TIM_Base_Start_IT(&DASHBOARD_BUTTONS_DEBOUNCE_TIMER_HANDLE);
}
