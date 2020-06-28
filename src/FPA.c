/*
 * FPA.c
 *
 *  Created on: 2020¦~6¤ë28¤é
 *      Author: andre
 */
#include "FPA.h"
uint32_t FPA_RTC=0,FPA_LTC=0;
void FPA_Handler(void)
{
	FPA_RTC = HAL_GetTick();
	if(FPA_RTC-FPA_LTC>500 && FPA_RTC-FPA_LTC<1000)
	{
		HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_14);
		CanHandle.pTxMsg->StdId = 0x211;
		CanHandle.pTxMsg->IDE = CAN_ID_STD;
		CanHandle.pTxMsg->DLC = 2;
		CanHandle.pTxMsg->Data[0] = 0x01;
		CanHandle.pTxMsg->Data[1] = 0x01;
		if(HAL_CAN_Transmit(&CanHandle, 0) != HAL_OK)
		{

		}
	}

	if(FPA_RTC-FPA_LTC>1000)
	{
		FPA_LTC = FPA_RTC;
		HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_14);
		CanHandle.pTxMsg->StdId = 0x211;
		CanHandle.pTxMsg->IDE = CAN_ID_STD;
		CanHandle.pTxMsg->DLC = 2;
		CanHandle.pTxMsg->Data[0] = 0x02;
		CanHandle.pTxMsg->Data[1] = 0x02;
		if(HAL_CAN_Transmit(&CanHandle, 0) != HAL_OK)
		{

		}
	}

	if(FPA_CAN_RXOK)
	{
		FPA_CAN_RXOK = 0;
		if(FPA_CAN_RXMSG == 0x01)
		{
			HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13, GPIO_PIN_SET);
		}
		if(FPA_CAN_RXMSG == 0x02)
		{
			HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13, GPIO_PIN_RESET);
		}
	}

	HAL_Delay(350);
}



