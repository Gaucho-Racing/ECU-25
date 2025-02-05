#include <stdbool.h>

#include "stateMachine.h"
#include "driving.h"
#include "main.h"

void drive_standby(StatusLump *status)
{
    // If not rtd, then go back to precharge_complete
    if (!HAL_GPIO_ReadPin(RTD_CONTROL_GPIO_Port, RTD_CONTROL_Pin))
        status->ECUState = PRECHARGE_COMPLETE;
    if(false /*Valid torque request*/ || false /*APPS stuff*/)
        status->ECUState = DRIVE_ACTIVE_IDLE;
    // TS ACTIVE, ACU shutdown, errors handled in CANdler.c
}

void drive_active_idle(StatusLump *status)
{
    // LOTS OF https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1214

    if (true /*Throttle is pushed*/)
       status->ECUState = DRIVE_ACTIVE_POWER;
    if (false /*No violation*/ && false /*Throttle is none*/ && false /*Speed > X mph*/ && false /*Not regenerating power*/)
       status->ECUState = DRIVE_ACTIVE_REGEN;
    if (false /*Violation*/)   // SEND WARNING TO DASH
       status->ECUState = DRIVE_STANDBY;
    if (false /*TS ACTIVE button disabled*/ || false/*ACU shutdown*/ || false/*Critical error*/)
        status->ECUState = TS_DISCHARGE_OFF;
}

void drive_active_power(StatusLump *status)
{
    // LOTS OF https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1214

    if (false /*Accelerator gradient plausibility violation*/)    // SEND WARNING TO DASH
        status->ECUState = DRIVE_STANDBY;
    if (false /*Brake over threshold*/ && false /*Throttle engaged*/)
        status->ECUState = DRIVE_STANDBY;
    if (false /*TS ACTIVE button disabled*/ || false /*ACU shutdown*/ || false /*Critical error*/)
        status->ECUState = TS_DISCHARGE_OFF;
}

void drive_active_regen(StatusLump *status)
{
    // LOTS OF https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1214
    // Some math in https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1253

    
    if (true /*Throttle engaged*/)
        status->ECUState = DRIVE_ACTIVE_POWER;
    if (false /*Settings say no regen braking*/)
        status->ECUState = DRIVE_ACTIVE_IDLE;
    if (false /*TS ACTIVE button disabled*/ || false /*ACU shutdown*/ || false /*Critical error*/)
       status->ECUState = TS_DISCHARGE_OFF;
}
