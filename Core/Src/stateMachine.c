#include "stateMachine.h"

stateMachineTick(State state, InformationToPassToState info)
{
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

/*
STARTUP STAGE 1:
ECU FLASH
When the Car is turned on, the Main ECU will read in the ECU flash from the SD card.
This will be the first state that the car will enter.
This is essential for the car to operate as the ECU flash contains the 
torque profiles, regen profiles, and traction control profiles.
*/
State ecu_flash()
{
    // Read SD Card
    // Import data
}

State glv_on()
{

}

State ts_precharge()
{

}

State ts_discharge_off()
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

State drive_active()
{

}

State drive_regen()
{

}

void readSDCard(VehicleTuneController& vehicleTuneController)
{
    // Read data in from the SD card so that everything is set right
}