#include <stdbool.h>
#include <stdio.h>

#include <fdcan.h>
#include "stm32g4xx_hal.h"
#include "utils.h"
#include "msgIDs.h"
#include "main.h"
#include "stateMachine.h"
#include "CANdler.h"
#include "driving.h"
#include "math.h"
#include "inverter.h"
#include "driving.h"

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
    if (close && !HAL_GPIO_ReadPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin)) // Avoid writing pins that are already written too
    {
        HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_SET);
    }
    else if (!close && HAL_GPIO_ReadPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin))
    {
        HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_RESET);
    }
}

bool checkBSEAPPSviolation(float throttle1, float throttle2, float pedalTravel, float brake)
{
    //Checks 2 * APPS_1 is within 10% of APPS_2 and break + throttle at the same time
    return fabs(throttle2 - throttle1 * 2) > throttle2 * 0.1 || (brake >= BSE_DEADZONE && pedalTravel >= 0.25);
}

void validateForwardTorqueRequest(uint16_t* tqr)
{
    int8_t deltaH;
    if(millis() - lastHeatCapacityUpdateMillis > 10){
        deltaH = 0.1;
    }
    else{
        deltaH = millis() - lastHeatCapacityUpdateMillis;
    }
    lastHeatCapacityUpdateMillis = millis();
    deltaH *=*   tqr**   tqr - MAX_CURRENT_FORWARD * MAX_CURRENT_FORWARD;
    heatCapacity += (heatCapacity + deltaH  > 0) ? deltaH : 0;
    if(heatCapacity > MAX_AMK_HEAT_CAP * 0.9 && *tqr > MAX_CURRENT_FORWARD * (1 - ((double)heatCapacity/MAX_AMK_HEAT_CAP - 0.9) / 0.1)){
        *tqr = MAX_CURRENT_FORWARD * (1 - ((double)heatCapacity/MAX_AMK_HEAT_CAP - 0.9) / 0.1);
    }
}

float vehicleSpeedMPH(void)
{
    return ((globalInverterData.msgOne.erpm / MOTOR_POLE_PAIRS) * 2 * M_PI * WHEEL_RADIUS_IN) / (GEAR_RATIO * 1056.0);  // TODO: Where did 1056 come from?
}

void sendBseAppsViolationMessage(void)
{
    uint8_t errorMap = 0x1;
    writeMessage(PrimaryBusCAN, MSG_DASH_WARNING_FLAGS, GR_DASH_PANEL, &errorMap, 1);
}

bool ACUError(ACU_Status_MsgTwo *acuMsgTwo)
{
    uint8_t notableBits = getBits(acuMsgTwo->Error_Warning_Bits, 0, 5);

    if (notableBits != 0x0)
    {
        char dashMsg[9];
        snprintf(dashMsg, 9, "ACUErr%hhX", notableBits);
        writeMessage(PrimaryBusCAN, MSG_DEBUG_2_0, GR_DASH_PANEL, (uint8_t*)dashMsg, 8);  // Not sending '\0'

        char steeringMsg[16];
        snprintf(steeringMsg, 16, "ACU Error -- %hhX", notableBits);
        writeMessage(DataBusCAN, MSG_DEBUG_FD, GR_STEERING_WHEEL, (uint8_t*)steeringMsg, 15);   // Not sending '\0'

        LOGOMATIC("ACU Error -- %hhX", notableBits);

        return acuMsgTwo->SDC_Voltage < 50;
    }

    return acuMsgTwo->SDC_Voltage < 50;
}

bool GRIError(Inverter_Status_Msg_Three *msgGriThree)
{
    if (msgGriThree->fault_map != 0x0)
    {
        char dashMsg[9];
        snprintf(dashMsg, 9, "GRIErr%hhX", msgGriThree->fault_map);
        writeMessage(PrimaryBusCAN, MSG_DEBUG_2_0, GR_DASH_PANEL, (uint8_t*)dashMsg, 8);  // Not sending '\0'

        char steeringMsg[16];
        snprintf(steeringMsg, 16, "GRI Error -- %hhX", msgGriThree->fault_map);
        writeMessage(DataBusCAN, MSG_DEBUG_FD, GR_STEERING_WHEEL, (uint8_t*)steeringMsg, 15);   // Not sending '\0'

        LOGOMATIC("GRI Error -- %hhX", msgGriThree->fault_map);

        return true;
    }

    return false;
}

bool ACUWarning(ACU_Status_MsgTwo *acuMsgTwo)
{
    uint8_t notableBits = getBits(acuMsgTwo->Error_Warning_Bits, 5, 3);

    if (notableBits != 0x0)
    {
        char dashMsg[9];
        snprintf(dashMsg, 9, "ACUWar%hhx", notableBits);
        writeMessage(PrimaryBusCAN, MSG_DEBUG_2_0, GR_DASH_PANEL, (uint8_t*)dashMsg, 8);    // Not sending '\0'

        char steeringMsg[18];
        snprintf(steeringMsg, 18, "ACU Warning -- %hhX", notableBits);
        writeMessage(DataBusCAN, MSG_DEBUG_FD, GR_STEERING_WHEEL, (uint8_t*)steeringMsg, 17);   // Not sending '\0'

        LOGOMATIC("ACU Warning -- %hhX", notableBits);

        return true;
    }

    return false;
}
