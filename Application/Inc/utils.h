#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>

#include "CANdler.h"

#define TICK_FREQ 1000

/**
Milliseconds

Returns the time in milliseconds.

Use in place of HAL Ticks for timing.
*/
uint32_t millis(void);

/**
Get Bit

@param number Byte in question
@param indexFromLeft What place bit to return from the left side [01234567]

Returns a byte that represents the bit (basically a bool).
*/
bool getBit(uint8_t number, uint8_t indexFromLeft);

/**
Get Bits (Plural)

@param number Byte in question
@param indexFromLeft What place bit to start looking at from the left side [01234567]
@param length How many bits after the index to return
@returns A byte that represents the bits in question.

It is complicated, look at the implementation.
*/
uint8_t getBits(uint8_t number, uint8_t indexFromLeft, uint8_t length);

/**
Set Software Latch

@param close Whether or not to close the latch

Essentially a nuanced flip-flop.
*/
void setSoftwareLatch(bool close);

/**
ACU Error

@param acuMsgTwo ACU Status Message 2
@return A byte representing the errorfulness.

Parses the message in question to see if there are any errors.
*/
bool ACUError(ACU_Status_MsgTwo *acuMsgTwo);

/**
GR Inverter Error

@param msgGriThree GR Inverter Status Message 3
@return A byte representing the errorfulness.

Parses the message in question to see if there are any errors.
*/
bool GRIError(Inverter_Status_Msg_Three *msgGriThree);

/**
ACU Warning

@param acuMsgTwo ACU Status Message 2
@return A byte representing the errorfulness.

Parses the message in question to see if there are any errors.
*/
bool ACUWarning(ACU_Status_MsgTwo *acuMsgTwo);

#endif // UTILS_H
