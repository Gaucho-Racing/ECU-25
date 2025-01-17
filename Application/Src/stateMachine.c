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

    // DO AND SEND THINGS
        
        // When the grounded low voltage system is turned on
        // the microcontroller has power, but the motor controller is not enabled.
        // This is the second state that the car will enter after the ECU Flash is complete.
        // Here it waits for the TS ACTIVE button to be pressed.

    /// if (/*TS ACTIVE button pressed*/)
    //     *state = precharge_engaged;
    // if (/*Parameters recieved*/)
    //    *state = REFLASH_TUNE;
}

void precharge_engaged(State* state)
{
    if (rateLimitOk(info)) {
        // DO AND SEND THINGS

        // ONLY INCLUDE BELOW LINE IF MESSAGE SENT
        info->lastMessageTick = HAL_GetTick();
    } else {
        // Nothing, cannot overload CANFD
    }

    // if (/*ACU precharge confirmation recieved*/)
    //     *state = PRECHARGING;
    // if (/*TS ACTIVE button disabled*/)
    //     *state = GLV_ON;
}

void precharging(State* state)
{
    if (rateLimitOk(info)) {
        // DO AND SEND THINGS
 
        // ONLY INCLUDE BELOW LINE IF MESSAGE SENT
        info->lastMessageTick = HAL_GetTick();
    } else {
        // Nothing, cannot overload CANFD
    }

    // if (/*ACU precharge success confirmation*/)
    //     *state = PRECHARGE_COMPLETE;
    // if (/*TS ACTIVE button disabled*/ || /*ACU precharge cancellation*/)
    //     *state = TS_DISCHARGE_OFF;
}

void precharge_complete(State* state)
{
    if (rateLimitOk(info)) {
        // DO AND SEND THINGS

        // ONLY INCLUDE BELOW LINE IF MESSAGE SENT
        info->lastMessageTick = HAL_GetTick();
    } else {
        // Nothing, cannot overload CANFD
    }

    // if (/*BRAKE on*/ && /*ReadyToDrive ON*/)
    //     *state = DRIVE_STANDBY;
    // if (/*TS ACTIVE button disabled*/ || /*ACU shutdown*/ || /*Critical error*/)
    //     *state = TS_DISCHARGE_OFF;
}

void drive_standby(State* state)
{
    if (rateLimitOk(info)) {
        // DO AND SEND THINGS

        // ONLY INCLUDE BELOW LINE IF MESSAGE SENT
        info->lastMessageTick = HAL_GetTick();
    } else {
        // Nothing, cannot overload CANFD
    }

    // if (/*Valid torque request*/)
    //     *state = DRIVE_ACTIVE_IDLE; // not sure if it's idle and not some other state
    // if (/*ReadyToDrive OFF*/)
    //     *state = PRECHARGE_COMPLETE;
    // if (/*TS ACTIVE button disabled*/ || /*ACU shutdown*/ || /*Critical error*/)
    //     *state = TS_DISCHARGE_OFF;
}

void drive_active_idle(State* state)
{
    // LOTS OF https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1214

    // if (/*Throttle is pushed*/)
    //    *state = DRIVE_ACTIVE_POWER;
    // if (/*No violation*/ && /*Throttle is none*/ && /*Speed > X mph*/ && /*Not regenerating power*/)
    //    *state = DRIVE_ACTIVE_REGEN;
    // if (/*Violation*/)   // SEND WARNING TO DASH
    //    *state = DRIVE_STANDBY;
    // if (/*TS ACTIVE button disabled*/ || /*ACU shutdown*/ || /*Critical error*/)
    //     *state = TS_DISCHARGE_OFF;
}

void drive_active_power(State* state)
{
    // LOTS OF https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1214

    // if (/*Accelerator gradient plausibility violation*/)    // SEND WARNING TO DASH
    //     *state = DRIVE_STANDBY;
    // if (/*Brake over threshold*/ && /*Throttle engaged*/)
    //     *state = DRIVE_STANDBY;
    // if (/*TS ACTIVE button disabled*/ || /*ACU shutdown*/ || /*Critical error*/)
    //     *state = TS_DISCHARGE_OFF;
}

void drive_active_regen(State* state)
{
    // LOTS OF https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1214
    // Some math in https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1253

    // if (/*Throttle engaged*/)
    //     *state = DRIVE_ACTIVE_POWER;
    // if (/*Settings say no regen braking*/)
    //     *state = DRIVE_ACTIVE_IDLE;
    // if (/*TS ACTIVE button disabled*/ || /*ACU shutdown*/ || /*Critical error*/)
    //     *state = TS_DISCHARGE_OFF;
}

void ts_discharge_off(State* state)
{
    // if (/*Main power off*/ && /*Errors resolved*/)
    //     *state = GLV_ON;
    // if (/*Main power off*/ && !/*Errors resolved*/)
    //     *state = ERROR;
}

void reflash_tune(State* state)
{
    // READ SD CARD INFORMATION INTO INFO and then
    // *state = GLV_ON;

    // if (/*Flash error*/)
    //     *state = ERROR;
}

void error(State* state)
{
    // if (/*Errors resolved*/)
    //     *state = GLV_ON;
}