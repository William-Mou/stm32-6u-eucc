/**
  ******************************************************************************
  * @file    CAN/CAN_Networking/Src/main.c 
  * @author  MCD Application Team
  * @version V1.3.3
  * @date    29-January-2016
  * @brief   This example shows how to configure the CAN peripheral 
  *          to send and receive CAN frames in normal mode. The sent frames 
  *          are used to control LEDs by pressing key push button.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "FPA.h"

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup CAN_Networking
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define KEY_PRESSED     0x01
#define KEY_NOT_PRESSED 0x00
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*uint8_t ubKeyNumber = 0x0;*/

__IO ITStatus UartReady = RESET;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
/*static void Error_Handler(void);*/
/*static void CAN_Config(void);*/
/*static void LED_Display(uint8_t LedStatus);*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  /*__attribute__ ((aligned (8))) */uint32_t txbox_num;
  ubKeyNumber = 0x0;
  RxMessagePending_flag = 0;
  totle_track_num = 0;
  HAL_Init();

  uint8_t press_cnt = 0;
  
  /* Configure the system clock to 180 MHz */
  SystemClock_Config();
  
  /*STM32f4_UART_DMA_Init(&UartHandle);*/

  /* Configure LED1, LED2, LED3 and LED4 */
/*  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);*/
  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);

  /* Configure Key Button */ 
  //BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
  /*##-1- Configure the CAN peripheral #######################################*/
  STM32f4_UART_DMA_Init(&UartHandle);

  //CAN_Config();
  STM32f4_CAN_Init(&CanHandle, &sFilterConfig, &TxMessage, &RxMessage);

  /*print_string_DMA("\r\n(1)\r\n");
  print_string_DMA("TX mailbox identifier register:");
  print_u32x_DMA(CanHandle.Instance->sTxMailBox[0].TIR);
  print_string_DMA("InterruptEnableReg:");
  print_u32x_DMA(CanHandle.Instance->IER);*/

  /*##-2- Start the Reception process and enable reception interrupt #########*/
  if(HAL_CAN_Receive_IT(&CanHandle, CAN_FIFO0) != HAL_OK)
  {
    /* Reception Error */
	  Error_Handler();
  }
  /* Infinite loop */
  /*print_string_DMA("(2)\r\n");
  print_string_DMA("ErrStateReg:");
  print_u32x_DMA(CanHandle.Instance->ESR);
  print_string_DMA("InterruptEnableReg:");
  print_u32x_DMA(CanHandle.Instance->IER);
  print_string_DMA("BUTTON State:");
  print_u32x_DMA(BSP_PB_GetState(BUTTON_KEY));

  print_string_DMA("(3)\r\n");*/

  while(1)
  {
	  FPA_Handler();
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  /* Activate the Over-Drive mode */
  HAL_PWREx_EnableOverDrive();
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete*/
  UartReady = SET;
  /* Turn LED3 on: Transfer in transmission process is correct */
  //BSP_LED_On(LED3);
}

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete*/
  UartReady = SET;
  /* Turn LED3 on: Transfer in reception process is correct */
  //BSP_LED_On(LED3);
}

/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  /* Turn LED3 on: Transfer error in reception/transmission process */
  //BSP_LED_On(LED3);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  while(1)
  {
  }
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef *CanHandle)
{
  print_string_DMA("C_RxCplt\r\n");
  if (1)//((CanHandle->pRxMsg->StdId == 0x321)&&(CanHandle->pRxMsg->IDE == CAN_ID_STD) && (CanHandle->pRxMsg->DLC == 8))
  {
    /*LED_Display(CanHandle->pRxMsg->Data[0]);*/
    ubKeyNumber = CanHandle->pRxMsg->Data[0];
    RxMessagePending_flag = 1;
    //print_string_DMA("StdId:");
    //print_u32d_DMA(CanHandle->pRxMsg->StdId);
    //print_string_DMA("ubKeyNumber:");
    //print_u8x_DMA(ubKeyNumber);
    //print_string_DMA("RxMessagePending_flag:");
    //print_32d_DMA(RxMessagePending_flag);
    print_string_DMA("RxMsg:");
    print_Array1D_8hex_DMA(CanHandle->pRxMsg->Data[0],CanHandle->pRxMsg->Data[1],CanHandle->pRxMsg->Data[2],CanHandle->pRxMsg->Data[3],\
    		CanHandle->pRxMsg->Data[4],CanHandle->pRxMsg->Data[5],CanHandle->pRxMsg->Data[6],CanHandle->pRxMsg->Data[7]);
    //根據RxMag判別Sensor量測結果
    //STM32f4_CAN_MeasureResultReceive_INT8wTW(CanHandle, &totle_track_num, DistanceOffset, TrackEdgeEst);
  }

  /*Receive*/
  if(HAL_CAN_Receive_IT(CanHandle, CAN_FIFO0) != HAL_OK)
  {
   /*Reception Error*/
    Error_Handler();
  }
  //print_string_DMA("RXcallback!\r\n");
}

/**
  * @brief  Turns ON/OFF the dedicated LED.
  * @param  LedStatus: LED number from 0 to 3
  * @retval None
  */
/*void LED_Display(uint8_t LedStatus)
{
   Turn OFF all LEDs
  HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_SET);
  
  switch(LedStatus)
  {
    case(1):
       Turn ON LED1
      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, GPIO_PIN_SET);
      break;
   
    case(2): 
       Turn ON LED2
      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_PIN, GPIO_PIN_SET);
      break;
 
    case(3): 
       Turn ON LED3
	  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_PIN, GPIO_PIN_SET);
      break;

    case(4): 
       Turn ON LED4
	  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_PIN, GPIO_PIN_RESET);
      break;
    default:
      break;
  }
}*/

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
