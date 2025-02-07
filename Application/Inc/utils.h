#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#define TICK_FREQ 100000 // overflow after 12 hrs runtime teehee

uint32_t millis(void);
uint8_t getBit(uint8_t number, uint8_t indexFromRight);
uint8_t get2Bits(uint8_t number, uint8_t indexFromRight);
uint8_t getBits(uint8_t number, uint8_t indexFromRight, uint8_t length)
void setSoftwareLatch(uint8_t close);
uint8_t ACUError(ACU_Status_MsgTwo *acuMsgTwo);
uint8_t GRIError(Inverter_Status_Msg_Three *msgGriThree);
uint8_t ACUWarning(ACU_Status_MsgTwo *acuMsgTwo);
#endif // UTILS_H
