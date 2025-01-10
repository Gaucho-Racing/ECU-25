#include <stdbool.h>
#include "stm32g4xx_hal.h"
#include "stateMachine.h"

void stateMachineTick(State* state, InformationToPassToState info)
{
    switch(*state) {
        case GLV_ON:
            glv_on(state, info);
            break;
        case PRECHARGE_ENGAGED:
            precharge_engaged(state, info);
            break;
        case PRECHARGING:
            precharging(state, info);
            break;
        case PRECHARGE_COMPLETE:
            precharge_complete(state, info);
            break;
        case DRIVE_STANDBY:
            drive_standby(state, info);
            break;
        case DRIVE_ACTIVE_IDLE:
            drive_active_idle(state, info);
            break;
        case DRIVE_ACTIVE_POWER:
            drive_active_power(state, info);
            break;
        case DRIVE_ACTIVE_REGEN:
            drive_active_regen(state, info);
            break;
        case TS_DISCHARGE_OFF:
            ts_discharge_off(state, info);
            break;
        case REFLASH_TUNE:
            reflash_tune(state, info);
            break;
        case ERRORSTATE:
        default:
            error(state, info);
        break;
    }
}

void glv_on(State* state, InformationToPassToState info)
{
    if (rateLimitOk(info)) {
        // DO AND SEND THINGS

        // ONLY INCLUDE BELOW LINE IF MESSAGE SENT
        info.lastMessageTick = HAL_GetTick();
    } else {
        // Nothing, cannot overload CANFD
    }
}

void precharge_engaged(State* state, InformationToPassToState info)
{
    if (rateLimitOk(info)) {
        // DO AND SEND THINGS

        // ONLY INCLUDE BELOW LINE IF MESSAGE SENT
        info.lastMessageTick = HAL_GetTick();
    } else {
        // Nothing, cannot overload CANFD
    }
}

void precharging(State* state, InformationToPassToState info)
{
    if (rateLimitOk(info)) {
        // DO AND SEND THINGS
 
        // ONLY INCLUDE BELOW LINE IF MESSAGE SENT
        info.lastMessageTick = HAL_GetTick();
    } else {
        // Nothing, cannot overload CANFD
    }
}

void precharge_complete(State* state, InformationToPassToState info)
{
    if (rateLimitOk(info)) {
        // DO AND SEND THINGS

        // ONLY INCLUDE BELOW LINE IF MESSAGE SENT
        info.lastMessageTick = HAL_GetTick();
    } else {
        // Nothing, cannot overload CANFD
    }
}

void drive_standby(State* state, InformationToPassToState info)
{
    if (rateLimitOk(info)) {
        // DO AND SEND THINGS

        // ONLY INCLUDE BELOW LINE IF MESSAGE SENT
        info.lastMessageTick = HAL_GetTick();
    } else {
        // Nothing, cannot overload CANFD
    }
}

void drive_active_idle(State* state, InformationToPassToState info)
{
    if (rateLimitOk(info)) {
        // DO AND SEND THINGS

        // ONLY INCLUDE BELOW LINE IF MESSAGE SENT
        info.lastMessageTick = HAL_GetTick();
    } else {
        // Nothing, cannot overload CANFD
    }
}

void drive_active_power(State* state, InformationToPassToState info)
{
    if (rateLimitOk(info)) {
        // DO AND SEND THINGS

        // ONLY INCLUDE BELOW LINE IF MESSAGE SENT
        info.lastMessageTick = HAL_GetTick();
    } else {
        // Nothing, cannot overload CANFD
    }
}

void drive_active_regen(State* state, InformationToPassToState info)
{
    if (rateLimitOk(info)) {
        // DO AND SEND THINGS

        // ONLY INCLUDE BELOW LINE IF MESSAGE SENT
        info.lastMessageTick = HAL_GetTick();
    } else {
        // Nothing, cannot overload CANFD
    }
}

void ts_discharge_off(State* state, InformationToPassToState info)
{
    if (rateLimitOk(info)) {
        // DO AND SEND THINGS

        // ONLY INCLUDE BELOW LINE IF MESSAGE SENT
        info.lastMessageTick = HAL_GetTick();
    } else {
        // Nothing, cannot overload CANFD
    }
}

void reflash_tune(State* state, InformationToPassToState info)
{
    if (rateLimitOk(info)) {
        // DO AND SEND THINGS

        // ONLY INCLUDE BELOW LINE IF MESSAGE SENT
        info.lastMessageTick = HAL_GetTick();
    } else {
        // Nothing, cannot overload CANFD
    }    
}

void error(State* state, InformationToPassToState info)
{
    if (rateLimitOk(info)) {
        // DO AND SEND THINGS

        // ONLY INCLUDE BELOW LINE IF MESSAGE SENT
        info.lastMessageTick = HAL_GetTick();
    } else {
        // Nothing, cannot overload CANFD
    }
}

_Bool rateLimitOk(InformationToPassToState info)
{
    return HAL_GetTick() - info.lastMessageTick > 1000 / info.communicationFrequency;
}