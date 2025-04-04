#include <stdbool.h>
#include <stdio.h>

#include <fdcan.h>
#include "stm32g4xx_hal.h"
#include "utils.h"
#include "msgIDs.h"
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
    if (close && !HAL_GPIO_ReadPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin))
    {
        HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_SET);
    }
    else if (!close && HAL_GPIO_ReadPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin))
    {
        HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_RESET);
    }
}

bool ACUError(ACU_Status_MsgTwo *acuMsgTwo)
{
    uint8_t value[8] = "ACUErr!?";
    value[7] = getBits(acuMsgTwo->Error_Warning_Bits, 0, 5);

    if(getBits(acuMsgTwo->Error_Warning_Bits, 0, 5) != 0x0)
    {
        writeMessage(PrimaryBusCAN, MSG_DEBUG_2_0, GR_DASH_PANEL, value, 8);

        char steeringMsg[18];
        snprintf(steeringMsg, 18, "ACU Error -- %hhX", value);
        writeMessage(DataBusCAN, MSG_DEBUG_FD, GR_STEERING_WHEEL, (uint8_t*)steeringMsg, 17);

        return acuMsgTwo->SDC_Voltage < 50;
    }

    return acuMsgTwo->SDC_Voltage < 50;
}

bool GRIError(Inverter_Status_Msg_Three *msgGriThree)
{
    uint8_t value[8] = "GRIErr!?";
    value[7] = msgGriThree->fault_map;

    if (msgGriThree->fault_map != 0x0)
    {
        writeMessage(PrimaryBusCAN, MSG_DEBUG_2_0, GR_DASH_PANEL, value, 8);

        char steeringMsg[18];
        snprintf(steeringMsg, 18, "GRI Error -- %hhX", msgGriThree->fault_map);
        writeMessage(DataBusCAN, MSG_DEBUG_FD, GR_STEERING_WHEEL, (uint8_t*)steeringMsg, 17);

        return true;
    }

    return false;
}

bool ACUWarning(ACU_Status_MsgTwo *acuMsgTwo)
{
    uint8_t value[8] = "ACUWar!?";
    value[7] = getBits(acuMsgTwo->Error_Warning_Bits, 5, 3);

    if (getBits(acuMsgTwo->Error_Warning_Bits, 5, 3) != 0x0)
    {
        writeMessage(PrimaryBusCAN, MSG_DEBUG_2_0, GR_DASH_PANEL, value, 8);

        char steeringMsg[20];
        snprintf(steeringMsg, 20, "ACU Warning -- %hhX", getBits(acuMsgTwo->Error_Warning_Bits, 5, 3));
        writeMessage(DataBusCAN, MSG_DEBUG_FD, GR_STEERING_WHEEL, (uint8_t*)steeringMsg, 19);
        return true;
    }

    return false;
}
