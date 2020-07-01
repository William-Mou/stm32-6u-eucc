/*
 * FPA.h
 *
 *  Created on: 2020¦~6¤ë28¤é
 *      Author: andre
 */

#ifndef FPA_H_
#define FPA_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

#define getOP(DATA) (DATA>>4) & 0xF
#define getVAL(DATA) (DATA&0xF)
#define TelID(OP,VAL) (OP<<4|VAL)

#define FPAACK TelID(1,1)
#define FPANACK TelID(1,0)
#define FPAPWRON TelID(2,1)
#define FPAPWROFF TelID(2,0)
#define HEATERON TelID(4,1)
#define HEATEROFF TelID(4,0)

extern CAN_HandleTypeDef CanHandle;
extern volatile int8_t FPA_CAN_RXOK;
extern volatile int8_t FPA_CAN_RXMSG;
extern volatile uint32_t FPA_CAN_RXId;
void FPA_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* FPA_H_ */
