#include <stdbool.h>
#include "stm32g4xx_hal.h"
#include "stateMachine.h"

StatusLump globalStatus = {
    .ECUState = GLV_ON,
    .StatusBits = {0}
    
};

void stateMachineTick(void)
{
    switch(globalStatus.ECUState) {
        case GLV_ON:
            glv_on(&globalStatus);
            break;
        case PRECHARGE_ENGAGED:
            precharge_engaged(&globalStatus);
            break;
        case PRECHARGING:
            precharging(&globalStatus);
            break;
        case PRECHARGE_COMPLETE:
            precharge_complete(&globalStatus);
            break;
        case DRIVE_STANDBY:
            drive_standby(&globalStatus);
            break;
        case DRIVE_ACTIVE_IDLE:
            drive_active_idle(&globalStatus);
            break;
        case DRIVE_ACTIVE_POWER:
            drive_active_power(&globalStatus);
            break;
        case DRIVE_ACTIVE_REGEN:
            drive_active_regen(&globalStatus);
            break;
        case TS_DISCHARGE_OFF:
            ts_discharge_off(&globalStatus);
            break;
        case REFLASH_TUNE:
            reflash_tune(&globalStatus);
            break;
        case ERRORSTATE:
        default:
            error(&globalStatus);
        break;
    }
}

void glv_on(StatusLump *status) {

    // When the grounded low voltage system is turned on
    // the microcontroller has power, but the motor controller is not enabled.
    // This is the second state that the car will enter after the ECU Flash is complete.
    // Here it waits for the TS ACTIVE button to be pressed.

    if (status->TractiveSystemVoltage > 60) {
        status->ECUState = TS_DISCHARGE_OFF;
        return;
    }

    status->ECUState = PRECHARGE_ENGAGED;
}

void precharge_engaged(StatusLump *status)
{
    if (status->Set /*ACU precharge confirmation recieved*/)
        status->ECUState = PRECHARGING;
    if (true /*TS ACTIVE button disabled*/)
        status->ECUState = GLV_ON;
}

void precharging(StatusLump *status)
{
    // ACU precharge status
    // TS Active button
    if (true /*ACU precharge success confirmation*/)
        status->ECUState = PRECHARGE_COMPLETE;
    if (false /*TS ACTIVE button disabled*/ || false /*ACU precharge cancellation*/)
        status->ECUState = TS_DISCHARGE_OFF;
}

void precharge_complete(StatusLump *status)
{
    if (false /*BRAKE on*/ && false /*ReadyToDrive ON*/)
        status->ECUState = DRIVE_STANDBY;
    if (false /*TS ACTIVE button disabled*/ || false /*ACU shutdown*/ || false /*Critical error*/)
        status->ECUState = TS_DISCHARGE_OFF;
}

void drive_standby(StatusLump *status)
{
    if (true /*Valid torque request*/)
        status->ECUState = DRIVE_ACTIVE_IDLE; // not sure if it's idle and not some other state
    if (false /*ReadyToDrive OFF*/)
        status->ECUState = PRECHARGE_COMPLETE;
    if (false /*TS ACTIVE button disabled*/ || false/*ACU shutdown*/ || false/*Critical error*/)
        status->ECUState = TS_DISCHARGE_OFF;
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

void ts_discharge_off(StatusLump *status)
{
    if (true /*Main power off*/ && true /*Errors resolved*/)
        status->ECUState = GLV_ON;
    if (false /*Main power off*/ && !true/*Errors resolved*/)
        status->ECUState = ERROR;
}

void reflash_tune(StatusLump *status)
{
    // READ SD CARD INFORMATION INTO INFO and then
    status->ECUState = GLV_ON;

    if (true /*Flash error*/)
        status->ECUState = ERROR;
}

void error(StatusLump *status)
{
    
    if (true /*Errors resolved*/)
        status->ECUState = GLV_ON;
}
