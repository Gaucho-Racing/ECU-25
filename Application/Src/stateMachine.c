#include "stateMachine.h"

void stateMachineTick(State* state, InformationToPassToState info)
{
    switch(*state) {
        case GLV_OFF:
            glv_off(state, info);
            break;
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
        case ERROR:
        default:
            error(state, info);
        break;
    }
}

void glv_off(State* state, InformationToPassToState info)
{

}

void glv_on(State* state, InformationToPassToState info)
{

}

void precharge_engaged(State* state, InformationToPassToState info)
{

}

void precharging(State* state, InformationToPassToState info)
{

}

void precharge_complete(State* state, InformationToPassToState info)
{

}

void drive_standby(State* state, InformationToPassToState info)
{

}

void drive_active_idle(State* state, InformationToPassToState info)
{

}

void drive_active_power(State* state, InformationToPassToState info)
{

}

void drive_active_regen(State* state, InformationToPassToState info)
{

}

void ts_discharge_off(State* state, InformationToPassToState info)
{

}

void reflash_tune(State* state, InformationToPassToState info)
{
    
}

void error(State* state, InformationToPassToState info)
{

}
