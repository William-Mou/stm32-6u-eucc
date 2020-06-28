/*
 * HAL_UART_out.c
 *
 *  Created on: 2014�~11��5��
 *      Author: ELied
 *      �Y�n�b�s��project���[�JUART Function�A�������T�����H�U�B�J�C
(1)
a.�NHAL_UART_out.c & HAL_UART_out.h���O�[�Jsrc & include��Ƨ��C
b.�N�n�ϥ�UART�禡��.c�ɤ��[�J���Y�� #include "HAL_UART_out.h"�C
c.�b�ϥ�UART�禡�e�n������STM32f4_UART_DMA_Init(&UartHandle);��STM32f4_UART_Init(&UartHandle);(�H�O�_�nDMA�\��A�M�w�έ��Ӫ�l�ƨ禡�C)
(2)
a.�bmain.c���ᤩUartReady�ܼƪ�l�ȡG__IO ITStatus UartReady = RESET;
b.�bmain.h���ŧi���cUartHandle�P�ܼ�UartReady�C
UART_HandleTypeDef UartHandle;
__IO ITStatus UartReady;
c.�bstm32f4xx_it.c���ŧi���cSpiHandle&UartHandle(���ϥ�SPI���_�禡�~�ݭn�ŧiSpiHandle&)
extern SPI_HandleTypeDef SpiHandle;
extern UART_HandleTypeDef UartHandle;
(3)
a.�N�ϥ�UART���n�����_�禡(Interrupt Handler)�[�Jstm32f4xx_it.c�ɮ׷��C�O�o�]�n�bstm32f4xx_it.h���Y�ɤ���禡�i��ŧi�C
b.�@��]�t�H�U�ɮ�(�Y�n�ҥ�UART_DMA�\��N�n�[�JDMA�禡)
void USART1_IRQHandler(void);
void DMA2_Stream5_IRQHandler(void);
void DMA2_Stream7_IRQHandler(void);
(4)���B�J�PUartReady�L�kSET���������Y
a.�Nmain.c���[�JUART callback funtion�A�ðO�o�bmain.h���Y�ɤ���禡�i��ŧi�C
���禡�s�b��void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma)�禡���A���`�ӻ��Y���[�J���禡�sĶ���|�����C���ѩ�HAL_Driver�̭����ŧi�o�X�Ө禡���P�W�禡(_weak����)�A����Ƥ��e��

�ťաA�G�sĶ�����|�����A���bDMA�I�sCallback Function��UartReady�ܼƫo���|�Q�]��SET�A�ɭP�d�bwhile()�j����L�k���X�C
b.�@��]�t�H�U�ɮ�
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle);
 */

#include <stdio.h>
#include "stm32f429xx.h"
#include "main.h"

/*#include "xComplex.h"*/

#ifdef __GNUC__
	#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

/*#define MaxPrintByte	(50)
char temp_buffer[MaxPrintByte] ="";*/

/* UART handler declaration */
//UART_HandleTypeDef UartHandle;

/* Buffer used for transmission */
//uint8_t UART_TxBuffer[] = " **** UART_TwoBoards_ComPolling ****  **** UART_TwoBoards_ComPolling ****  **** UART_TwoBoards_ComPolling **** ";
uint8_t UART_TxBuffer[] = " **** **** ****";
/* Buffer used for reception */
uint8_t UART_RxBuffer[10];

void STM32f4_UART_Init(UART_HandleTypeDef *huart){
	/*##-1.2- Configure the UART peripheral ######################################*/
	/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
	/* UART1 configured as follow:
		- Word Length = 8 Bits
		- Stop Bit = One Stop bit
		- Parity = None
		- BaudRate = 9600 baud
		- Hardware flow control disabled (RTS and CTS signals) */
	USARTx_CLK_ENABLE();
	USARTx_RX_GPIO_CLK_ENABLE();
	//USARTx_TX_GPIO_CLK_ENABLE();�PUSARTx_RX_GPIO_CLK_ENABLE()�ۦP
	//__HAL_RCC_USART1_FORCE_RESET();

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pull = GPIO_PULLUP; // �ϥΤW�Թq��
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP; // �]�m���Х�
	GPIO_InitStructure.Pin = USARTx_TX_PIN; // �]�w�� 09 �}
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; // �]�w GPIO �t�׬� 50 MHz
	GPIO_InitStructure.Alternate = USARTx_TX_AF; // �]�w GPIO �t�׬� 50 MHz	//UART TX&RX Alternate function�ҬۦP �G�Ȼݪ�l�Ƥ@��
	HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure); // �M�ΥH�W GPIO �]�m�A�ê�l�� GPIOC

	/******** �]�w PC11 �� Rx �Х�  ********/
	GPIO_InitStructure.Pull = GPIO_PULLUP; // �ϥΤW�Թq��
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT; // �]�m���Х�
	GPIO_InitStructure.Pin = GPIO_PIN_10; // �]�w�� 10 �}
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; // �]�w GPIO �t�׬� 50 MHz
	GPIO_InitStructure.Alternate = USARTx_RX_AF; // �]�w GPIO �t�׬� 50 MHz	//UART TX&RX Alternate function�ҬۦP �G�Ȼݪ�l�Ƥ@��
	HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure); // �M�ΥH�W GPIO �]�m�A�ê�l�� GPIOC

	huart->Instance          = USARTx;

	huart->Init.BaudRate     = 921600;
	huart->Init.WordLength   = UART_WORDLENGTH_8B;
	huart->Init.StopBits     = UART_STOPBITS_1;
	huart->Init.Parity       = UART_PARITY_NONE;
	huart->Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	huart->Init.Mode         = UART_MODE_TX_RX;
	huart->Init.OverSampling = UART_OVERSAMPLING_16;

	HAL_UART_Init(huart);
	/*if(HAL_UART_Init(&UartHandle) != HAL_OK)
	{
		Error_Handler();
	}*/

	/*##-3.2- Start the transmission process #####################################*/
	/* While the UART in reception process, user can transmit data through
	  "aTxBuffer" buffer */
	HAL_UART_Transmit(huart, (uint8_t*)UART_TxBuffer, TXBUFFERSIZE, 5000);
	/*if(HAL_UART_Transmit(&UartHandle, (uint8_t*)UART_TxBuffer, TXBUFFERSIZE, 5000)!= HAL_OK)
	{
		Error_Handler();
	}*/
}

void STM32f4_UART_DMA_Init(UART_HandleTypeDef *huart){
	/*##-1- Configure the UART peripheral ######################################*/
	/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
	/* UART1 configured as follow:
		- Word Length = 8 Bits
		- Stop Bit = One Stop bit
		- Parity = None
		- BaudRate = 9600 baud
		- Hardware flow control disabled (RTS and CTS signals) */
	static DMA_HandleTypeDef hdma_tx;
	static DMA_HandleTypeDef hdma_rx;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable USART1 clock */
	USARTx_CLK_ENABLE();
	/* Enable DMA1 clock */
	DMAx_CLK_ENABLE();
	USARTx_RX_GPIO_CLK_ENABLE();
	//USARTx_TX_GPIO_CLK_ENABLE();//�PUSARTx_RX_GPIO_CLK_ENABLE()�ۦP
	//__HAL_RCC_USART1_FORCE_RESET();

	/*##-2- Configure peripheral GPIO ##########################################*/
	GPIO_InitStructure.Pull = GPIO_PULLUP; // �ϥΤW�Թq��
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP; // �]�m���Х�
	GPIO_InitStructure.Pin = USARTx_TX_PIN; // �]�w�� 09 �}
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; // �]�w GPIO �t�׬� 50 MHz
	GPIO_InitStructure.Alternate = USARTx_TX_AF; // �]�w GPIO �t�׬� 50 MHz	//UART TX&RX Alternate function�ҬۦP �G�Ȼݪ�l�Ƥ@��
	HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure); // �M�ΥH�W GPIO �]�m�A�ê�l�� GPIOC

	/******** �]�w PC11 �� Rx �Х�  ********/
	//GPIO_InitStructure.Pull = GPIO_PULLUP; // �ϥΤW�Թq��
	//GPIO_InitStructure.Mode = GPIO_MODE_AF_PP; // �]�m���Х�
	GPIO_InitStructure.Pin = GPIO_PIN_10; // �]�w�� 10 �}
	//GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; // �]�w GPIO �t�׬� 50 MHz
	GPIO_InitStructure.Alternate = USARTx_RX_AF; // �]�w GPIO �t�׬� 50 MHz	//UART TX&RX Alternate function�ҬۦP �G�Ȼݪ�l�Ƥ@��
	HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure); // �M�ΥH�W GPIO �]�m�A�ê�l�� GPIOC

	huart->Instance          = USARTx;

	huart->Init.BaudRate     = 921600;
	huart->Init.WordLength   = UART_WORDLENGTH_8B;
	huart->Init.StopBits     = UART_STOPBITS_1;
	huart->Init.Parity       = UART_PARITY_NONE;
	huart->Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	huart->Init.Mode         = UART_MODE_TX_RX;
	huart->Init.OverSampling = UART_OVERSAMPLING_16;

	HAL_UART_Init(huart);

	/*##-3- Configure the DMA streams ##########################################*/
	/* Configure the DMA handler for Transmission process */
	hdma_tx.Instance                 = USARTx_TX_DMA_STREAM;

	hdma_tx.Init.Channel             = USARTx_TX_DMA_CHANNEL;
	hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
	hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	hdma_tx.Init.Mode                = DMA_NORMAL;
	hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
	hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
	hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;

	HAL_DMA_Init(&hdma_tx);

	/* Associate the initialized DMA handle to the the UART handle */
	__HAL_LINKDMA(huart, hdmatx, hdma_tx);

	/* Configure the DMA handler for Transmission process */
	hdma_rx.Instance                 = USARTx_RX_DMA_STREAM;

	hdma_rx.Init.Channel             = USARTx_RX_DMA_CHANNEL;
	hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
	hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	hdma_rx.Init.Mode                = DMA_NORMAL;
	hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
	hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	hdma_rx.Init.MemBurst            = DMA_MBURST_INC4;
	hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC4;

	HAL_DMA_Init(&hdma_rx);

	/* Associate the initialized DMA handle to the the UART handle */
	__HAL_LINKDMA(huart, hdmarx, hdma_rx);

	/*##-4- Configure the NVIC for DMA #########################################*/
	/* NVIC configuration for DMA transfer complete interrupt (USARTx_TX) */
	HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 3, 1);
	HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

	/* NVIC configuration for DMA transfer complete interrupt (USARTx_RX) */
	HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);

	/* NVIC configuration for USART TC interrupt */
	HAL_NVIC_SetPriority(USART1_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

	/*##-5- Start the transmission process #####################################*/
	/* While the UART in reception process, user can transmit data through
	  "UART_TxBuffer" buffer */
	HAL_UART_Transmit_DMA(huart, (uint8_t*)UART_TxBuffer, TXBUFFERSIZE);
	while (UartReady != SET){}
	UartReady = RESET;
}

/*void printUsart(const char *str){
	while(*str!='\0'){
		USART_SendData(USART2,(uint8_t)*(str++));
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){};
	}
}
*/
void print_u32d(unsigned int int32){
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "%u\r\n", int32);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit(&UartHandle, (uint8_t*)temp_buffer, print_cnt, 5000);
}
void print_u8x(uint8_t int8){
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "0x%02X\r\n", int8);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit(&UartHandle, (uint8_t*)temp_buffer, print_cnt, 5000);
}
void print_u32x(unsigned int int32){
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "0x%08X\r\n", int32);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit(&UartHandle, (uint8_t*)temp_buffer, print_cnt, 5000);
}
void print_32d(int int32){
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "%d\r\n", int32);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit(&UartHandle, (uint8_t*)temp_buffer, print_cnt, 5000);
}

void print_u32d_DMA(unsigned int int32){
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "%u\r\n", int32);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}
void print_u8x_DMA(uint8_t int8){
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "0x%02X\r\n", int8);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}
void print_u32x_DMA(unsigned int int32){
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "0x%08X\r\n", int32);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}
void print_32d_DMA(int int32){
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "%d\r\n", int32);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}

void printResult_DMA(uint16_t MeasureCnt, uint8_t SlaveNum, uint8_t Axis, int MeasureResult){
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "%u S%u A%u %d\r\n", MeasureCnt, SlaveNum, Axis, MeasureResult);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}

void print_float(float f1){
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "%f\r\n", f1);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit(&UartHandle, (uint8_t*)temp_buffer, print_cnt, 5000);
}

void print_float_DMA(float fvar1){
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "%f\r\n", fvar1);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}/*print_u8x_DMA(print_cnt);while(1);*/
	/*for(int i=0;i<MaxPrintByte;i+=8){print_Array1D_8int_DMA(temp_buffer[i],temp_buffer[i+1],temp_buffer[i+2],temp_buffer[i+3],temp_buffer[i+4],temp_buffer[i+5],temp_buffer[i+6],temp_buffer[i+7]);}*/
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}

void print_double_DMA(double dvar1){
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "%5.25f\r\n", dvar1);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}

void print_MatrixDoubleElement_DMA(double e1){
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "%5.7f\t", e1);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}

void print_string(char str[]){
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "%s", str);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit(&UartHandle, (uint8_t*)temp_buffer, print_cnt, 5000);
}

void print_string_DMA(char str[]){
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "%s", str);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}

void print_MatrixSize_DMA(int Iexamp, int row, int column){
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "Example #%d: Size of Matrix \"c\", Row:%d Column:%d\r\n", Iexamp, row, column);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}

void print_string_DMA_nosptf(void){
	uint8_t print_cnt=0;
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}

void print_CubeEquCoef_DMA(double a3, double a2, double a1, double a0){
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "[%6.4f\t%6.4f\t%6.4f\t%6.4f]\r\n", a3, a2, a1, a0);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}

void print_QuarticEquCoef_DMA(double a4, double a3, double a2, double a1, double a0)
{
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "[%6.4f\t%6.4f\t%6.4f\t%6.4f\t%6.4f]\r\n", a4, a3, a2, a1, a0);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}
void print_IntTypeElement_DMA(int d1){
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "%d\t", d1);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}

/*void print_complex_DMA(xComplex *Complex)
{
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "%7.3f %7.3fi\r\n", Complex->real,Complex->img);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}*/

void print_range_DMA(int start_index, int end_index)
{
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "[ %d -> %d ]\r\n", start_index,end_index);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}

void print_range_double_DMA(double start_index, double end_index)
{
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "[ %4.2f -> %4.2f ]\r\n", start_index,end_index);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}

void print_Array1D_8int_DMA(int a7, int a6, int a5, int a4, int a3, int a2, int a1, int a0)
{
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "[%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d]\r\n", a7, a6, a5, a4, a3, a2, a1, a0);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}

void print_Array1D_8hex_DMA(int a7, int a6, int a5, int a4, int a3, int a2, int a1, int a0)
{
	uint8_t print_cnt=0;
	sprintf(temp_buffer, "[%02X\t%02X\t%02X\t%02X\t%02X\t%02X\t%02X\t%02X]\r\n", a7, a6, a5, a4, a3, a2, a1, a0);
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}

void print_Array1D_8int_binary_DMA(int a7, int a6, int a5, int a4, int a3, int a2, int a1, int a0)
{
	uint8_t print_cnt=0;
	sprintf(temp_buffer, \
	"[" BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "\
		BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\r\n",\
	BYTE_TO_BINARY(a7), BYTE_TO_BINARY(a6), BYTE_TO_BINARY(a5),	BYTE_TO_BINARY(a4), BYTE_TO_BINARY(a3), BYTE_TO_BINARY(a2), BYTE_TO_BINARY(a1), BYTE_TO_BINARY(a0));
	while(temp_buffer[print_cnt]!='\0'){print_cnt++;}
	HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)temp_buffer, print_cnt);
	while (UartReady != SET){}
	UartReady = RESET;
}
