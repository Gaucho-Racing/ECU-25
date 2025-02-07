#include <stdbool.h>
#include "stm32g4xx_hal.h"
#include "stateMachine.h"
#include "driving.h"
#include "main.h"
#include "utils.h"

StatusLump globalStatus = {
    .ECUState = GLV_ON,
    .StatusBits = {0}
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
    
    HAL_GPIO_WritePin(SOFTWARE_OK_GPIO_Port, SOFTWARE_OK_Pin, GPIO_PIN_SET);
    // When the grounded low voltage system is turned on
    // the microcontroller has power, but the motor controller is not enabled.
    // This is the second state that the car will enter after the ECU Flash is complete.
    // Here it waits for the TS ACTIVE button to be pressed.

    // GLV handled in CANdler.c::handleCANMessage, under case MSG_DASH_STATUS
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
    //if (true /*TS ACTIVE button disabled*/)
    //   status->ECUState = GLV_ON;
    
}

void precharging(StatusLump *status)
{
    //max voltage is 600, 580 is about when it will be done
    // This might be unnecessary, we already check for success confirmation in CAN
    /*
    if (status->TractiveSystemVoltage > 580)
        status->ECUState = PRECHARGE_COMPLETE;
    */
    //TS ACTIVE button disabled || ACU precharge cancellation --> TS_DISCHARGE_OFF is handled in CANdler.c
    // line 66, 176
}

void precharge_complete(StatusLump *status)
{

    // If front, rear, and rtd, then go to DRIVE_STANDBY
    if (analogRead(BRAKE_F_SIGNAL_GPIO_Port, BRAKE_F_SIGNAL_Pin) && analogRead(BRAKE_R_SIGNAL_GPIO_Port, BRAKE_R_SIGNAL_Pin) && HAL_GPIO_ReadPin(RTD_CONTROL_GPIO_Port, RTD_CONTROL_Pin))
    {
        status->ECUState = DRIVE_STANDBY;
    }
    // TS ACTIVE, ACU shutdown, errors handled in CANdler.c
}

void ts_discharge_off(StatusLump *status)
{
    if(HAL_GPIO_ReadPin(SOFTWARE_OK_GPIO_Port, SOFTWARE_OK_Pin)){
        HAL_GPIO_WritePin(SOFTWARE_OK_GPIO_Port, SOFTWARE_OK_Pin, GPIO_PIN_RESET);
    }

    if(dischargeStartMillis == -1){
        dischargeStartMillis = millis();
    }
    
    if ((millis() - dischargeStartMillis) > 5000) // Magic number :)
    {
        status->ECUState = ERRORSTATE;  // This sends us to ERRORSTATE with powered TS? Yes, ERRORSTATE will send it back if voltage >= 60
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
    
    if(status->TractiveSystemVoltage >= 60)
    {
        status->ECUState = TS_DISCHARGE_OFF;
    }
    
    /* Only error resolved when MSG_ACU AND GRI says we are good -> Handled in CANdler*/
}
