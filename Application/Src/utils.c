#include <stdbool.h>

#include <fdcan.h>
#include "stm32g4xx_hal.h"
#include "utils.h"
#include "main.h"
#include "stateMachine.h"
#include "CANdler.h"

uint32_t millis(void)
{
    return HAL_GetTick() * (1000 / TICK_FREQ);
}

bool getBit(uint8_t number, uint8_t indexFromLeft)
{
    return (number >> (7 - indexFromLeft)) & 0x1;
}

uint8_t getBits(uint8_t number, uint8_t indexFromLeft, uint8_t length)
{
    return (number >> (8 - indexFromLeft - length)) & ((1 << length) - 1);
}

void setSoftwareLatch(bool close)
{
    // TODO Confirm that having read/write of the same pin is ok
    if (close && !HAL_GPIO_ReadPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin))
    {
        HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_SET);
    }
    else if (close == 0 && HAL_GPIO_ReadPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin))
    {
        HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_RESET);
    }
}

bool ACUError(ACU_Status_MsgTwo *acuMsgTwo)
{
    uint8_t value[8];
    value[0] = getBits(acuMsgTwo->Error_Warning_Bits, 0, 5);
    value[1] = 'ACUErr!';

    if(getBits(acuMsgTwo->Error_Warning_Bits, 0, 5) != 0x0){
        writeMessage(PrimaryBusCAN, DEBUG, GR_DASH_PANEL, (uint8_t*)getBits(acuMsgTwo->Error_Warning_Bits, 0, 5), 8);
    }

    return getBits(acuMsgTwo->Error_Warning_Bits, 0, 5) != 0x0 || acuMsgTwo->SDC_Voltage < 50;
}

bool GRIError(Inverter_Status_Msg_Three *msgGriThree)
{
    return msgGriThree->fault_map != 0x00;
}

bool ACUWarning(ACU_Status_MsgTwo *acuMsgTwo)
{
    return getBits(acuMsgTwo->Error_Warning_Bits, 5, 3) != 0x0;
}
