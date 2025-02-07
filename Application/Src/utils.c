#include "stm32g4xx_hal.h"
#include "utils.h"
#include "main.h"
#include "stateMachine.h"
#include "CANdler.h"

uint32_t millis(void) {
    return HAL_GetTick() * (1000/TICK_FREQ);
}

uint8_t getBit(uint8_t number, uint8_t indexFromRight)
{
    return (number >> (7 - indexFromRight)) & 0x01;
}

uint8_t get2Bits(uint8_t number, uint8_t indexFromRight)
{
    return (number >> (7 - indexFromRight)) & 0x03; // 0b11
}

void setSoftwareLatch(uint8_t close)
{
    if (close != 0 && !HAL_GPIO_ReadPin(SOFTWARE_OK_GPIO_Port, SOFTWARE_OK_Pin))
    {
        HAL_GPIO_WritePin(SOFTWARE_OK_GPIO_Port, SOFTWARE_OK_Pin, GPIO_PIN_SET);
    }
    else if (HAL_GPIO_ReadPin(SOFTWARE_OK_GPIO_Port, SOFTWARE_OK_Pin))
    {
        HAL_GPIO_WritePin(SOFTWARE_OK_GPIO_Port, SOFTWARE_OK_Pin, GPIO_PIN_RESET);
    }
}

uint8_t ACUError(ACU_Status_MsgTwo *acuMsgTwo)
{
    if (acuMsgTwo->Error_Warning_Bits != 0x00)
    {
        //
    }
}