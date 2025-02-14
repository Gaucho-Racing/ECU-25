#include "stm32g4xx_hal.h"
#include "utils.h"
#include "main.h"
#include "stateMachine.h"
#include "CANdler.h"

uint32_t millis(void) {
    return HAL_GetTick() * (1000/TICK_FREQ);
}

uint8_t getBit(uint8_t number, uint8_t indexFromLeft)
{
    return (number >> (7 - indexFromLeft)) & 0x01;
}

uint8_t getBits(uint8_t number, uint8_t indexFromLeft, uint8_t length)
{
    return (number >> (8 - indexFromLeft - length)) & ((1 << length) - 1);
}

void setSoftwareLatch(uint8_t close)
{
    if (close != 0 && !HAL_GPIO_ReadPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin))
    {
        HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_SET);
    }
    else if (HAL_GPIO_ReadPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin))
    {
        HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_RESET);
    }
}

uint8_t ACUError(ACU_Status_MsgTwo *acuMsgTwo)
{
    // Double check later
    return getBits(acuMsgTwo->Error_Warning_Bits, 0, 5) != 0x00 || acuMsgTwo->SDC_Voltage/10 < 5;
}

uint8_t GRIError(Inverter_Status_Msg_Three *msgGriThree)
{
    return msgGriThree->fault_map != 0x00;
}

uint8_t ACUWarning(ACU_Status_MsgTwo *acuMsgTwo)
{
    // Double check later
    return getBits(acuMsgTwo->Error_Warning_Bits, 5, 3);
}
