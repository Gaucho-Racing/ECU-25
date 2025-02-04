#include <stdbool.h>
#include "stm32g4xx_hal.h"
#include "stateMachine.h"
#include "driving.h"

StatusLump globalStatus = {
    .ECUState = GLV_ON,
    .StatusBits = {0}
    
};

void stateMachineTick(void)
{
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

void glv_on(StatusLump *status) {

    // When the grounded low voltage system is turned on
    // the microcontroller has power, but the motor controller is not enabled.
    // This is the second state that the car will enter after the ECU Flash is complete.
    // Here it waits for the TS ACTIVE button to be pressed.

    if (true /*TS ACTIVE from CAN*/)
        status->ECUState = PRECHARGE_ENGAGED;
}

void precharge_engaged(StatusLump *status)
{
    // Where ACU precharge?? Not in CAN or ACU Status.
    // if(true) left as is for now, straight to precharging
    if (true /*ACU precharge confirmation recieved*/)
       status->ECUState = PRECHARGING;
    //TS ACTIVE botton disabled --> GLV_ON is handled in CANdler.c
    //if (true /*TS ACTIVE button disabled*/)
    //   status->ECUState = GLV_ON;
    
}

void precharging(StatusLump *status)
{
    if (true /*ACU precharge success confirmation*/)
        status->ECUState = PRECHARGE_COMPLETE;
    if (false /*TS ACTIVE button disabled*/ || false /*ACU precharge cancellation*/)
        status->ECUState = TS_DISCHARGE_OFF;
}

void precharge_complete(StatusLump *status)
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    raw = HAL_ADC_GetValue(&hadc1);

    if (false /*BRAKE on*/ && false /*ReadyToDrive ON*/)
        status->ECUState = DRIVE_STANDBY;
    if (false /*TS ACTIVE button disabled*/ || false /*ACU shutdown*/ || false /*Critical error*/)
        status->ECUState = TS_DISCHARGE_OFF;
}

void ts_discharge_off(StatusLump *status)
{
    if (true /*Main power off*/ && true /*Errors resolved*/)
        status->ECUState = GLV_ON;
    if (false /*Main power off*/ && !true/*Errors resolved*/)
        status->ECUState = ERROR;
}

void reflash_tune(StatusLump *status)
{
    // READ SD CARD INFORMATION INTO INFO and then
    status->ECUState = GLV_ON;

    if (true /*Flash error*/)
        status->ECUState = ERROR;
}

void error(StatusLump *status)
{
    

    if (true /*Errors resolved*/)
        status->ECUState = GLV_ON;
}
