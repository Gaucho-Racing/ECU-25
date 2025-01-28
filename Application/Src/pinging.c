#include <stdint.h>

#include "stm32g4xx_hal.h"
#include "pinging.h"
#include "fdcan.h"
#include "msgIDs.h"
#include "grIDs.h"


const uint8_t pingIDs[] = {GR_ACU,
                           GR_GR_INVERTER_1,
                           GR_GR_INVERTER_2,
                           GR_GR_INVERTER_3,
                           GR_GR_INVERTER_4,
                           GR_FAN_CONTROLLER_1,
                           GR_FAN_CONTROLLER_2,
                           GR_FAN_CONTROLLER_3,
                           GR_FAN_CONTROLLER_4,
                           GR_DASH_PANEL,
                           GR_STEERING_WHEEL};
uint32_t pingTimes[PINGCOUNT] = {0};

static bool pingHasReturned[PINGCOUNT] = {false};


void pingSchedule(void)
{
    static bool hasPinged = false;

    if(HAL_GetTick() % (PINGTIMEOUT*100) >= (PINGTIMEOUT*100)/2) {
        hasPinged = false;
        return;
    }
    if(hasPinged == true) {
        return;
    }

    // check for timed out pings
    for(int i = 0; i < PINGCOUNT; i++) {
        if(!pingHasReturned[i]) {
            pingTimes[i] = PINGTIMEOUT*100;
        }

        pingHasReturned[i] = false;
    }

    uint32_t tick = HAL_GetTick();
    writeMessage(1, MSG_PING, GR_ALL, (uint8_t *)&tick, sizeof(uint32_t));
}

void respondToPing(uint8_t destID, uint32_t timestamp) {
    for(int i = 0; i < PINGCOUNT; i++) {
        if(destID == pingIDs[i]){
            pingTimes[i] = HAL_GetTick() - timestamp;
            pingHasReturned[i] = true;
            return;
        }
    }
    Error_Handler();
}
