#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#define TICK_FREQ 100000 // overflow after 12 hrs runtime teehee

uint32_t millis(void);
uint8_t getBit(uint8_t number, uint8_t indexFromRight);
uint8_t get2Bits(uint8_t number, uint8_t indexFromRight);

#endif // UTILS_H
