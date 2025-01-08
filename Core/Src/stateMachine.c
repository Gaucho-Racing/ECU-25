#include "stateMachine.h"

void stateMachineTick(State state)
{
    while(1)
        switch(state) {
            case ECU_FLASH:
                state = ecu_flash();
                break;
            case GLV_ON:
                state = glv_on();
                break;
            case TS_PRECHARGE:
                state = ts_precharge();
                break;
            case TS_DISCHARGE_OFF:
                state = ts_discharge_off();
                break;
            case PRECHARGING:
                state = precharging();
                break;
            case PRECHARGE_COMPLETE:
                state = precharge_complete();
                break;
            case DRIVE_STANDBY:
                state = drive_standby();
                break;
            case DRIVE_ACTIVE:
                state = drive_active();
                break;
            case DRIVE_REGEN:
                state = drive_regen();
                break;
            case ERROR:
            default:
                state = error();
                break;
        }
    }
}