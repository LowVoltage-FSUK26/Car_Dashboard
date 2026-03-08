#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "main.h"
#include "stdint.h"

#include "Nextion_int.h"
#include "Nextion_config.h"



uint8_t TransmitBuffer[NEXTION_COMMAND_MAX_LENGTH] = {0};
uint16_t TransmitBufferLength = 0;
char END[NEXTION_COMMAND_END_LENGTH]               = {NEXTION_COMMAND_END};
uint8_t Current_Page=1;





//-----------------------------------------------------------------------------------------------------------------------------
//void Nextion_Init(UART_HandleTypeDef *huart) // This function will be removed if I didn't think of something to do with it
//{
//    TransmitBufferLength = (uint16_t)sprintf((char *)TransmitBuffer, "baud=%d", NEXTION_UART_BAUDRATE);
//    Nextion_SendCommand(huart, (uint8_t *)TransmitBuffer);
//
//    // Crulial delay to allow Nextion to change baudrate
//    HAL_Delay(100);
//
//    huart->Init.BaudRate = NEXTION_UART_BAUDRATE;
//    if (HAL_UART_Init(huart) != HAL_OK)
//    {
//      Error_Handler();
//    }
//}
//-----------------------------------------------------------------------------------------------------------------------------






void Nextion_AddComp(Nextion *nex, char *_objname, uint8_t _id, char *_page)
{
	strcpy(nex->CompName, _objname);
	nex->CompID = _id;
	strcpy(nex->CompPage, _page);
}


void Nextion_SendCommand(UART_HandleTypeDef *huart, uint8_t *command)
{
	HAL_UART_Transmit(huart, command, TransmitBufferLength, 1000);
	Nextion_EndCommand(huart);
}

void Nextion_EndCommand(UART_HandleTypeDef *huart)
{
	HAL_UART_Transmit(huart, (const uint8_t *)END, NEXTION_COMMAND_END_LENGTH, 1000);
}


void Nextion_SetText(UART_HandleTypeDef *huart, Nextion *nex, char *text)
{
	TransmitBufferLength = (uint8_t)sprintf((char *)TransmitBuffer, "%s.txt=\"%s\"", nex->CompName, text);
	Nextion_SendCommand(huart, TransmitBuffer);
}


void Nextion_SetVal(UART_HandleTypeDef *huart, Nextion *nex, uint32_t val)
{
	TransmitBufferLength = (uint8_t)sprintf((char *)TransmitBuffer, "%s.val=%lu", nex->CompName, val);
	Nextion_SendCommand(huart, TransmitBuffer);
}
//********************************************************NEW CODE ***********************************************************//
void Nextion_AddGlobalComp(Nextion *nex, char *_objname, uint8_t _id) //currently unused
{
	strcpy(nex->CompName, _objname);
	nex->CompID = _id;
}
void Nextion_Set_Vis(UART_HandleTypeDef *huart,Nextion*nex, uint8_t state)
{
	TransmitBufferLength = (uint8_t)sprintf((char *)TransmitBuffer, "vis %s,%d", nex->CompName, state);
	Nextion_SendCommand(huart, TransmitBuffer);
}
void Nextion_SetPage(UART_HandleTypeDef *huart,char*page)
{
	TransmitBufferLength = (uint8_t)sprintf((char *)TransmitBuffer, "page %s", page);
	Nextion_SendCommand(huart, TransmitBuffer);
}
void Nextion_CreateDiagnostics(Nextion T[32], Nextion V[32])
{
    for(uint8_t i = 0; i < 32; i++)
    {
        const char *page = (i < 16) ? "Diagnostics3" : "Diagnostics4";

        /* -------- T Components -------- */
        sprintf(T[i].CompName, "T%d", i + 1);
        T[i].CompID = 0;
        strcpy(T[i].CompPage, page);

        /* -------- V Components -------- */
        sprintf(V[i].CompName, "V%d", i + 1);
        V[i].CompID = 0;
        strcpy(V[i].CompPage, page);
    }
}


