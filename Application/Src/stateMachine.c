#include <stdbool.h>
#include "stm32g4xx_hal.h"
#include "stateMachine.h"
#include "driving.h"
#include "main.h"
#include "utils.h"
#include "adc.h"

volatile StatusLump globalStatus = {
    .ECUState = GLV_ON,
    .StatusBits = {0},
    .PowerLevelTorqueMap = 15
};

volatile uint8_t numberOfBadMessages = 0;
int32_t dischargeStartMillis = -1;

volatile SteerSettings steerSettings = {0};

void stateMachineTick(void)
{
    if (numberOfBadMessages >= 3)   // Magic value, 3 is bad arbitrarily (prime)
    {
        numberOfBadMessages = 0;
        Error_Handler();
    }

    if(globalStatus.ECUState != TS_DISCHARGE_OFF){
        dischargeStartMillis = -1;
    }

    switch(globalStatus.ECUState) {
        case GLV_ON:
            glv_on();
            break;
        case PRECHARGE_ENGAGED:
            precharge_engaged();
            break;
        case PRECHARGING:
            precharging();
            break;
        case PRECHARGE_COMPLETE:
            precharge_complete();
            break;
        case DRIVE_STANDBY:
            drive_standby();
            break;
        case DRIVE_ACTIVE_IDLE:
            drive_active_idle();
            break;
        case DRIVE_ACTIVE_POWER:
            drive_active_power();
            break;
        case DRIVE_ACTIVE_REGEN:
            drive_active_regen();
            break;
        case TS_DISCHARGE_OFF:
            ts_discharge_off();
            break;
        case REFLASH_TUNE:
            reflash_tune();
            break;
        case ERRORSTATE:
        default:
            error();
        break;
    }
}

void glv_on(void)
{
    // For safety
    if(globalStatus.TractiveSystemVoltage >= 60)
    {
        globalStatus.ECUState = TS_DISCHARGE_OFF;
    }

    // Close software latch, should be error free at this point Also reset power level
    setSoftwareLatch(1);
    globalStatus.PowerLevelTorqueMap &= 0xF0;
    // TS on handled in CANdler.c::handleCANMessage, under case MSG_DASH_STATUS
}

void precharge_engaged(void)
{
    // For safety
    if(globalStatus.TractiveSystemVoltage >= 60)
    {
        globalStatus.ECUState = TS_DISCHARGE_OFF;
    }
    // ACU confirmation is IR-, handled in CANdler.c
    //TS ACTIVE botton disabled --> GLV_ON is handled in CANdler.c
    
}

void precharging(void)
{

    //TS ACTIVE button disabled || ACU precharge cancellation --> TS_DISCHARGE_OFF is handled in CANdler.c
    // line 66, 176
}

void precharge_complete(void)
{
    // If front, rear, and rtd, then go to DRIVE_STANDBY handled in CAN
    // TS ACTIVE, ACU shutdown, errors handled in CANdler.c
}


void ts_discharge_off(void)
{
    setSoftwareLatch(0);

    if(dischargeStartMillis == -1){
        dischargeStartMillis = millis();
    }
    
    if ((millis() - dischargeStartMillis) > 5000) // Magic number :)
    {
        globalStatus.ECUState = ERRORSTATE;  // ERRORSTATE will send it back if voltage >= 60
    }
    // Other stuff handled in can
}

void reflash_tune(void)
{
    // READ SD CARD INFORMATION INTO INFO and then
    globalStatus.ECUState = GLV_ON;

    if (true /*Flash error*/)
    {
        globalStatus.ECUState = ERRORSTATE;
    }
}

void error(void)
{
    // DISCHARGE IF TS VOLTAGE >= 60 for some reason
    
    setSoftwareLatch(0);

    if(globalStatus.TractiveSystemVoltage >= 60)
    {
        globalStatus.ECUState = TS_DISCHARGE_OFF;
    }
    
    /* Only error resolved when MSG_ACU AND GRI says we are good -> Handled in CANdler*/
}
