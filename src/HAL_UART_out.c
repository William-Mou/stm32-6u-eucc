/*
 * HAL_UART_out.c
 *
 *  Created on: 2014年11月5日
 *      Author: ELied
 *      若要在新的project當中加入UART Function，必須正確完成以下步驟。
(1)
a.將HAL_UART_out.c & HAL_UART_out.h分別加入src & include資料夾。
b.將要使用UART函式的.c檔內加入標頭檔 #include "HAL_UART_out.h"。
c.在使用UART函式前要先執行STM32f4_UART_DMA_Init(&UartHandle);或STM32f4_UART_Init(&UartHandle);(以是否要DMA功能，決定用哪個初始化函式。)
(2)
a.在main.c中賦予UartReady變數初始值：__IO ITStatus UartReady = RESET;
b.在main.h當中宣告結構UartHandle與變數UartReady。
UART_HandleTypeDef UartHandle;
__IO ITStatus UartReady;
c.在stm32f4xx_it.c中宣告結構SpiHandle&UartHandle(有使用SPI中斷函式才需要宣告SpiHandle&)
extern SPI_HandleTypeDef SpiHandle;
extern UART_HandleTypeDef UartHandle;
(3)
a.將使用UART必要的中斷函式(Interrupt Handler)加入stm32f4xx_it.c檔案當中。記得也要在stm32f4xx_it.h標頭檔中對函式進行宣告。
b.一般包含以下檔案(若要啟用UART_DMA功能就要加入DMA函式)
void USART1_IRQHandler(void);
void DMA2_Stream5_IRQHandler(void);
void DMA2_Stream7_IRQHandler(void);
(4)此步驟與UartReady無法SET有直接關係
a.將main.c當中加入UART callback funtion，並記得在main.h標頭檔中對函式進行宣告。
此函式存在於void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma)函式中，正常來說若未加入此函式編譯器會報錯。但由於HAL_Driver裡面有宣告這幾個函式的同名函式(_weak類型)，但函數內容為

空白，故編譯器不會報錯，但在DMA呼叫Callback Function時UartReady變數卻不會被設為SET，導致卡在while()迴圈當中無法跳出。
b.一般包含以下檔案
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
	//USARTx_TX_GPIO_CLK_ENABLE();與USARTx_RX_GPIO_CLK_ENABLE()相同
	//__HAL_RCC_USART1_FORCE_RESET();

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pull = GPIO_PULLUP; // 使用上拉電阻
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP; // 設置為覆用
	GPIO_InitStructure.Pin = USARTx_TX_PIN; // 設定第 09 腳
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; // 設定 GPIO 速度為 50 MHz
	GPIO_InitStructure.Alternate = USARTx_TX_AF; // 設定 GPIO 速度為 50 MHz	//UART TX&RX Alternate function皆相同 故僅需初始化一次
	HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure); // 套用以上 GPIO 設置，並初始化 GPIOC

	/******** 設定 PC11 為 Rx 覆用  ********/
	GPIO_InitStructure.Pull = GPIO_PULLUP; // 使用上拉電阻
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT; // 設置為覆用
	GPIO_InitStructure.Pin = GPIO_PIN_10; // 設定第 10 腳
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; // 設定 GPIO 速度為 50 MHz
	GPIO_InitStructure.Alternate = USARTx_RX_AF; // 設定 GPIO 速度為 50 MHz	//UART TX&RX Alternate function皆相同 故僅需初始化一次
	HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure); // 套用以上 GPIO 設置，並初始化 GPIOC

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
	//USARTx_TX_GPIO_CLK_ENABLE();//與USARTx_RX_GPIO_CLK_ENABLE()相同
	//__HAL_RCC_USART1_FORCE_RESET();

	/*##-2- Configure peripheral GPIO ##########################################*/
	GPIO_InitStructure.Pull = GPIO_PULLUP; // 使用上拉電阻
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP; // 設置為覆用
	GPIO_InitStructure.Pin = USARTx_TX_PIN; // 設定第 09 腳
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; // 設定 GPIO 速度為 50 MHz
	GPIO_InitStructure.Alternate = USARTx_TX_AF; // 設定 GPIO 速度為 50 MHz	//UART TX&RX Alternate function皆相同 故僅需初始化一次
	HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure); // 套用以上 GPIO 設置，並初始化 GPIOC

	/******** 設定 PC11 為 Rx 覆用  ********/
	//GPIO_InitStructure.Pull = GPIO_PULLUP; // 使用上拉電阻
	//GPIO_InitStructure.Mode = GPIO_MODE_AF_PP; // 設置為覆用
	GPIO_InitStructure.Pin = GPIO_PIN_10; // 設定第 10 腳
	//GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; // 設定 GPIO 速度為 50 MHz
	GPIO_InitStructure.Alternate = USARTx_RX_AF; // 設定 GPIO 速度為 50 MHz	//UART TX&RX Alternate function皆相同 故僅需初始化一次
	HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure); // 套用以上 GPIO 設置，並初始化 GPIOC

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
