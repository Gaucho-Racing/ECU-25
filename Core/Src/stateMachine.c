#include "stateMachine.h"

/* WHY DO THE FILES NOT LINK AHH MY CMAKE IS BAD */

stateMachineTick(State* state, InformationToPassToState info)
{
    switch(state) {
        case GLV_OFF:
            *state = glv_off();
            break
        case GLV_ON:
            *state = glv_on();
            break
        case PRECHARGE_ENGAGED:
            *state = precharge_engaged();
            break
        case PRECHARGING:
            *state = precharging();
            break
        case PRECHARGE_COMPLETE:
            *state = precharge_complete();
            break
        case DRIVE_STANDBY:
            *state = drive_standby();
            break
        case DRIVE_ACTIVE_IDLE:
            *state = drive_active_idle();
            break
        case DRIVE_ACTIVE_POWER:
            *state = drive_active_power();
            break
        case DRIVE_ACTIVE_REGEN:
            *state = drive_active_regen();
            break
        case TS_DISCHARGE_OFF:
            *state = ts_discharge_off();
            break
        case REFLASH_TUNE:
            *state = reflash_tune();
            break
        case ERROR:
        default:
            error();
        break
    }
}

State glv_off()
{

}

State glv_on()
{

}

State precharge_engaged()
{

}

State precharging()
{

}

State precharge_complete()
{

}

State drive_standby()
{

}

State drive_active_idle()
{

}

State drive_active_power()
{

}

State drive_active_regen()
{

}

State ts_discharge_off()
{

}

State reflash_tune()
{
    
}

State error()
{

}