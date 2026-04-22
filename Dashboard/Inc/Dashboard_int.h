#ifndef DASHBOARD_INT_H
#define DASHBOARD_INT_H


typedef struct
{
    uint32_t mili_Sec;
    
    uint32_t Seconds;
    uint8_t Sec_dec_1;
    uint8_t Sec_dec_2;
    uint8_t Sec_dig_1;
    uint8_t Sec_dig_2;
    
    
    uint32_t Minutes;
    uint8_t Min_d1;
    uint8_t Min_d2;

}StopWatch;

typedef union
{
	uint8_t bytes[8];

	struct {

		uint16_t slave[4];	//Total Voltage/TEMP of each slave
							//In case of last frame in chain, It holds TOTAL Battery Voltage/TEMP
	} frame;

} BMS_CAN_Frame_t;

//BMS_CAN_Frame_t RxData;


#define				STOPWATCH_COUNTING			0
#define				STOPWATCH_PAUSED			1
#define				STOPWATCH_STOPED			2




void Dashboard_Init(void);
void LabTimer_CallBack_ISR();
void LabTimerStoped();
void LabTimer_Buttons_Callback(TIM_HandleTypeDef *htim, uint16_t GPIO_Pin);
void Debouncing_CallBack(TIM_HandleTypeDef *htim);
//*******************************************************************NEW CODE*********************************************************************//
void Nextion_NextPage(); //--> for buttons done
void Nextion_PrevPage(); //--> for buttons done



void CAN_Message(CAN_HandleTypeDef *hcan);


#endif // DASHBOARD_INT_H
// End of file
