#include "stm32g4xx_hal.h"
#include "utils.h"
#include "main.h"

uint32_t millis(void) {
    return HAL_GetTick() * (1000/TICK_FREQ);
}

uint8_t getBit(uint8_t number, uint8_t indexFromRight)
{
    return (number >> (7 - indexFromRight)) & 0b1;
}

uint8_t get2Bits(uint8_t number, uint8_t indexFromRight)
{
    return (number >> (7 - indexFromRight)) & 0b11;
}
