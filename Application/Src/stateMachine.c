#include <stdbool.h>
#include "stm32g4xx_hal.h"
#include "stateMachine.h"
#include "driving.h"
#include "main.h"
#include "utils.h"
#include "adc.h"

StatusLump globalStatus = {
    .ECUState = GLV_ON,
    .StatusBits = {0},
    .PowerLevelTorqueMap = 15
};

uint8_t numberOfBadMessages = 0;
int32_t dischargeStartMillis = -1;

SteerSettings steerSettings = {0};

void stateMachineTick(void)
{
    if (numberOfBadMessages >= 3)   // Magic value, 3 is bad
    {
        numberOfBadMessages = 0;
        Error_Handler();
    }

    if(globalStatus.ECUState != TS_DISCHARGE_OFF){
        dischargeStartMillis = -1;
    }

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

void glv_on(StatusLump *status)
{
    // For safety
    if(status->TractiveSystemVoltage >= 60)
    {
        status->ECUState = TS_DISCHARGE_OFF;
    }

    // Close software latch, should be error free at this point Also reset power level
    setSoftwareLatch(1);
    global.PowerLevelTorqueMap &= 0xF0;
    // TS on handled in CANdler.c::handleCANMessage, under case MSG_DASH_STATUS
}

void precharge_engaged(StatusLump *status)
{
    // For safety
    if(status->TractiveSystemVoltage >= 60)
    {
        status->ECUState = TS_DISCHARGE_OFF;
    }
    // ACU confirmation is IR-, handled in CANdler.c
    //TS ACTIVE botton disabled --> GLV_ON is handled in CANdler.c
    
}

void precharging(StatusLump *status)
{
    UNUSED(status);

    //TS ACTIVE button disabled || ACU precharge cancellation --> TS_DISCHARGE_OFF is handled in CANdler.c
    // line 66, 176
}

void precharge_complete(StatusLump *status)
{
    UNUSED(status);
    // If front, rear, and rtd, then go to DRIVE_STANDBY handled in CAN
    // TS ACTIVE, ACU shutdown, errors handled in CANdler.c
}


void ts_discharge_off(StatusLump *status)
{
    setSoftwareLatch(0);

    if(dischargeStartMillis == -1){
        dischargeStartMillis = millis();
    }
    
    if ((millis() - dischargeStartMillis) > 5000) // Magic number :)
    {
        status->ECUState = ERRORSTATE;  // ERRORSTATE will send it back if voltage >= 60
    }
    // Other stuff handled in can
}

void reflash_tune(StatusLump *status)
{
    // READ SD CARD INFORMATION INTO INFO and then
    status->ECUState = GLV_ON;

    if (true /*Flash error*/)
    {
        status->ECUState = ERRORSTATE;
    }
}

void error(StatusLump *status)
{
    // DISCHARGE IF TS VOLTAGE >= 60 for some reason
    
    setSoftwareLatch(0);

    if(status->TractiveSystemVoltage >= 60)
    {
        status->ECUState = TS_DISCHARGE_OFF;
    }
    
    /* Only error resolved when MSG_ACU AND GRI says we are good -> Handled in CANdler*/
}
