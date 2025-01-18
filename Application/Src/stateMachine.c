#include <stdbool.h>
#include "stm32g4xx_hal.h"
#include "stateMachine.h"

void stateMachineTick(State* state)
{
    switch(*state) {
        case GLV_ON:
            glv_on(state);
            break;
        case PRECHARGE_ENGAGED:
            precharge_engaged(state);
            break;
        case PRECHARGING:
            precharging(state);
            break;
        case PRECHARGE_COMPLETE:
            precharge_complete(state);
            break;
        case DRIVE_STANDBY:
            drive_standby(state);
            break;
        case DRIVE_ACTIVE_IDLE:
            drive_active_idle(state);
            break;
        case DRIVE_ACTIVE_POWER:
            drive_active_power(state);
            break;
        case DRIVE_ACTIVE_REGEN:
            drive_active_regen(state);
            break;
        case TS_DISCHARGE_OFF:
            ts_discharge_off(state);
            break;
        case REFLASH_TUNE:
            reflash_tune(state);
            break;
        case ERRORSTATE:
        default:
            error(state);
        break;
    }
}

void glv_on(State* state) {

    // When the grounded low voltage system is turned on
    // the microcontroller has power, but the motor controller is not enabled.
    // This is the second state that the car will enter after the ECU Flash is complete.
    // Here it waits for the TS ACTIVE button to be pressed.

    if (true /*TS ACTIVE from CAN*/)
        *state = PRECHARGE_ENGAGED;
}

void precharge_engaged(State* state)
{
    if (true /*ACU precharge confirmation recieved*/)
        *state = PRECHARGING;
    if (true /*TS ACTIVE button disabled*/)
        *state = GLV_ON;
}

void precharging(State* state)
{
    if (true /*ACU precharge success confirmation*/)
        *state = PRECHARGE_COMPLETE;
    if (false /*TS ACTIVE button disabled*/ || false /*ACU precharge cancellation*/)
        *state = TS_DISCHARGE_OFF;
}

void precharge_complete(State* state)
{
    if (false /*BRAKE on*/ && false/*ReadyToDrive ON*/)
        *state = DRIVE_STANDBY;
    if (false /*TS ACTIVE button disabled*/ || false /*ACU shutdown*/ || false /*Critical error*/)
        *state = TS_DISCHARGE_OFF;
}

void drive_standby(State* state)
{
    if (true /*Valid torque request*/)
        *state = DRIVE_ACTIVE_IDLE; // not sure if it's idle and not some other state
    if (false /*ReadyToDrive OFF*/)
        *state = PRECHARGE_COMPLETE;
    if (false /*TS ACTIVE button disabled*/ || false/*ACU shutdown*/ || false/*Critical error*/)
        *state = TS_DISCHARGE_OFF;
}

void drive_active_idle(State* state)
{
    // LOTS OF https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1214

    if (true /*Throttle is pushed*/)
       *state = DRIVE_ACTIVE_POWER;
    if (false /*No violation*/ && false /*Throttle is none*/ && false /*Speed > X mph*/ && false /*Not regenerating power*/)
       *state = DRIVE_ACTIVE_REGEN;
    if (false /*Violation*/)   // SEND WARNING TO DASH
       *state = DRIVE_STANDBY;
    if (false /*TS ACTIVE button disabled*/ || false/*ACU shutdown*/ || false/*Critical error*/)
        *state = TS_DISCHARGE_OFF;
}

void drive_active_power(State* state)
{
    // LOTS OF https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1214

    if (false /*Accelerator gradient plausibility violation*/)    // SEND WARNING TO DASH
        *state = DRIVE_STANDBY;
    if (false /*Brake over threshold*/ && /*Throttle engaged*/)
        *state = DRIVE_STANDBY;
    if (false /*TS ACTIVE button disabled*/ || false /*ACU shutdown*/ || false /*Critical error*/)
        *state = TS_DISCHARGE_OFF;
}

void drive_active_regen(State* state)
{
    // LOTS OF https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1214
    // Some math in https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1253

    if (true /*Throttle engaged*/)
        *state = DRIVE_ACTIVE_POWER;
    if (false /*Settings say no regen braking*/)
        *state = DRIVE_ACTIVE_IDLE;
    if (false /*TS ACTIVE button disabled*/ || false /*ACU shutdown*/ || false /*Critical error*/)
        *state = TS_DISCHARGE_OFF;
}

void ts_discharge_off(State* state)
{
    if (true /*Main power off*/ && true /*Errors resolved*/)
        *state = GLV_ON;
    if (false /*Main power off*/ && !true/*Errors resolved*/)
        *state = ERROR;
}

void reflash_tune(State* state)
{
    // READ SD CARD INFORMATION INTO INFO and then
    *state = GLV_ON;

    if (true /*Flash error*/)
        *state = ERROR;
}

void error(State* state)
{
    if (true /*Errors resolved*/)
        *state = GLV_ON;
}
