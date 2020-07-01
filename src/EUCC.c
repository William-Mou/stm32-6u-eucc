/*
 * FPA.c
 *
 *  Created on: 2020¦~6¤ë28¤é
 *      Author: andre
 */
#include "EUCC.h"
uint32_t FPA_RTC=0,FPA_LTC=0;
uint8_t status = 0;

void EUCC_Handler(void)
{

	FPA_RTC = HAL_GetTick();
	if(FPA_RTC-FPA_LTC>500 && FPA_RTC-FPA_LTC<1000 && status == 0)
	{
		status = 1;
		HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_14);
		CanHandle.pTxMsg->StdId = 0x207;
		CanHandle.pTxMsg->IDE = CAN_ID_STD;
		CanHandle.pTxMsg->DLC = 1;
		CanHandle.pTxMsg->Data[0] = 0x21;
		//CanHandle.pTxMsg->Data[1] = 0x01;
		if(HAL_CAN_Transmit(&CanHandle, 0) != HAL_OK)
		{

		}
	}

	if(FPA_RTC-FPA_LTC>1000 && FPA_RTC-FPA_LTC<1500 && status == 1)
	{
		status = 2;
		HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_14);
		CanHandle.pTxMsg->StdId = 0x207;
		CanHandle.pTxMsg->IDE = CAN_ID_STD;
		CanHandle.pTxMsg->DLC = 1;
		CanHandle.pTxMsg->Data[0] = 0x41;
		//CanHandle.pTxMsg->Data[1] = 0x01;
		if(HAL_CAN_Transmit(&CanHandle, 0) != HAL_OK)
		{

		}
	}
	if(FPA_RTC-FPA_LTC>1500 && FPA_RTC-FPA_LTC<2000 && status == 2)
	{
		status = 3;
		HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_14);
		CanHandle.pTxMsg->StdId = 0x207;
		CanHandle.pTxMsg->IDE = CAN_ID_STD;
		CanHandle.pTxMsg->DLC = 1;
		CanHandle.pTxMsg->Data[0] = 0x40;
		//CanHandle.pTxMsg->Data[1] = 0x01;
		if(HAL_CAN_Transmit(&CanHandle, 0) != HAL_OK)
		{

		}
	}
	if(FPA_RTC-FPA_LTC>2000 && status == 3)
	{
		status = 0;
		FPA_LTC = FPA_RTC;
		HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_14);
		CanHandle.pTxMsg->StdId = 0x207;
		CanHandle.pTxMsg->IDE = CAN_ID_STD;
		CanHandle.pTxMsg->DLC = 1;
		CanHandle.pTxMsg->Data[0] = 0x20;
		//CanHandle.pTxMsg->Data[1] = 0x01;
		if(HAL_CAN_Transmit(&CanHandle, 0) != HAL_OK)
		{

		}
	}

	if(FPA_CAN_RXOK)
	{
		FPA_CAN_RXOK = 0;
		// OBC -> EUCC 001
		if(FPA_CAN_RXId == 0x001)
		{
		//		HAL_GPIO_WritePin(GPIOG,GPIO_PIN_13, GPIO_PIN_SET);
				HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_13);
				CanHandle.pTxMsg->StdId = 0x101;
				CanHandle.pTxMsg->IDE = CAN_ID_STD;
				CanHandle.pTxMsg->DLC = 2;
				CanHandle.pTxMsg->Data[0] = 0x1F;
		//		CanHandle.pTxMsg->Data[1] = 0x02;
				if(HAL_CAN_Transmit(&CanHandle, 0) != HAL_OK)
				{
				}
		}

		//uint32_t OPcode = getOP(FPA_CAN_RXMSG);

		//if (OPcode == 0x01)



		if(FPA_CAN_RXId && 0xF00 == 0x300)
		{
			HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_13);
		}


	}

	HAL_Delay(350);
}



