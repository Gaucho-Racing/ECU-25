#include "stateMachine.h"

void stateMachineTick(State* state, InformationToPassToState info)
{

    // Honestly I would just pass in the state as a pointer and modify it in each function so you don't need to return it
    // Also if there's any persistent data that needs to be passed between states then you can just change the function to return that info, 
    // and forcing the function to return the state can be annoying to deal with (although you can also just achieve this with another pointer)
    // If there's a good reason why we have to return State then just revert my commit - Bailey
    switch(*state) {
        case GLV_OFF:
            glv_off(state);
            break;
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
        case ERROR:
        default:
            error(state);
        break;
    }
}

void glv_off(State* state)
{

}

void glv_on(State* state)
{

}

void precharge_engaged(State* state)
{

}

void precharging(State* state)
{

}

void precharge_complete(State* state)
{

}

void drive_standby(State* state)
{

}

void drive_active_idle(State* state)
{

}

void drive_active_power(State* state)
{

}

void drive_active_regen(State* state)
{

}

void ts_discharge_off(State* state)
{

}

void reflash_tune(State* state)
{
    
}

void error(State* state)
{

}
