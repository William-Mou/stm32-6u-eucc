/*
 * HAL_UART_out.h
 *
 *  Created on: 2014¦~11¤ë5¤é
 *      Author: ELied
 */

#include "stm32f4xx_hal.h"
#include <stdio.h>
/*#include "xComplex.h"*/

#ifdef __cplusplus
extern "C" {
#endif

void STM32f4_UART_Init(UART_HandleTypeDef *huart);
void STM32f4_UART_DMA_Init(UART_HandleTypeDef *huart);
void print_u32d(unsigned int int32);
void print_u8x(uint8_t int8);
void print_u32x(unsigned int int32);
void print_32d(int int32);
void print_u32d_DMA(unsigned int int32);
void print_u8x_DMA(uint8_t int8);
void print_u32x_DMA(unsigned int int32);
void print_32d_DMA(int int32);
void printResult_DMA(uint16_t MeasureCnt, uint8_t SlaveNum, uint8_t Axis, int MeasureResult);

void print_float(float f1);
void print_float_DMA(float fvar1);
void print_double_DMA(double dvar1);
void print_MatrixDoubleElement_DMA(double e1);

void print_string(char str[]);
void print_string_DMA(char str[]);
void print_MatrixSize_DMA(int Iexamp, int row, int column);
void print_string_DMA_nosptf(void);
void print_CubeEquCoef_DMA(double a3, double a2, double a1, double a0);
void print_QuarticEquCoef_DMA(double a4, double a3, double a2, double a1, double a0);
void print_IntTypeElement_DMA(int d1);
/*void print_complex_DMA(xComplex *Complex);*/
void print_range_DMA(int start_index, int end_index);
void print_range_double_DMA(double start_index, double end_index);

void print_Array1D_8int_DMA(int a7, int a6, int a5, int a4, int a3, int a2, int a1, int a0);
void print_Array1D_8hex_DMA(int a7, int a6, int a5, int a4, int a3, int a2, int a1, int a0);
void print_Array1D_8int_binary_DMA(int a7, int a6, int a5, int a4, int a3, int a2, int a1, int a0);

/* User can use this section to tailor USARTx/UARTx instance used and associated
   resources */
/* Definition for USARTx clock resources */
#define USARTx                           USART1
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE();
#define DMAx_CLK_ENABLE()                __HAL_RCC_DMA2_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_9
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_AF                     GPIO_AF7_USART1
#define USARTx_RX_PIN                    GPIO_PIN_10
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_AF                     GPIO_AF7_USART1

/* Definition for USARTx's DMA */
#define USARTx_TX_DMA_CHANNEL            DMA_CHANNEL_4
#define USARTx_TX_DMA_STREAM             DMA2_Stream7
#define USARTx_RX_DMA_CHANNEL            DMA_CHANNEL_4
#define USARTx_RX_DMA_STREAM             DMA2_Stream5

/* Size of Transmission buffer */
#define TXBUFFERSIZE                     (COUNTOF(UART_TxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                     TXBUFFERSIZE

#define MaxPrintByte	(80)

char temp_buffer[MaxPrintByte];

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')
