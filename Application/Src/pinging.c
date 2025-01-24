#include <stdint.h>

#include "stm32g4xx_hal.h"
#include "pinging.h"
#include "fdcan.h"
#include "msgIDs.h"
#include "grIDs.h"

uint32_t pingTimesArray[256];

void pingSchedule()
{
    static uint8_t lastPingedItem = 10;
    uint8_t itemToPing = HAL_GetTick() % 250 / 23;

    if(lastCase)

    uint8_t bus = 1;
    uint16_t msgID = MSG_PING;
    uint8_t destID;
    uint32_t timeInMS = HAL_GetTick();
    uint8_t* data = &timeInMS;
    uint32_t len = 4;

    uint32_t ms = HAL_GetTick();
    switch ((HAL_GetTick() % 250 / 23)) {
        case 0:
            destID = GR_ACU;
            break;
        case 1:
            destID = GR_GR_INVERTER_1;
            break;
        case 2:
            destID = GR_GR_INVERTER_2;
            break;
        case 3:
            destID = GR_GR_INVERTER_3;
            break;
        case 4:
            destID = GR_GR_INVERTER_4;
            break;
        case 5:
            destID = GR_FAN_CONTROLLER_1;
            break;
        case 6:
            destID = GR_FAN_CONTROLLER_2;
            break;
        case 7:
            destID = GR_FAN_CONTROLLER_3;
            break;
        case 8:
            destID = GR_FAN_CONTROLLER_4;
            break;
        case 9:
            destID = GR_DASH_PANEL;
            break;
        case 10:
            destID = GR_STEERING_WHEEL;
            break;
    }
    
    pingTimestampsArray[destID] = timeInMS;
    writeMessage(bus, msgID, destID, data, len);
}

void respondToPing(uint8_t destID, uint32_t timestamp) {
    pingTimesArray[destID] = HAL_GetTick()
}