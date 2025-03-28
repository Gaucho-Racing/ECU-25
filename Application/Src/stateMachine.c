#include <stdbool.h>

#include "stm32g4xx_hal.h"
#include "stateMachine.h"
#include "driving.h"
#include "inverter.h"
#include "main.h"
#include "utils.h"
#include "adc.h"
#include "fdcan.h"
#include "msgIDs.h"
#include "grIDs.h"

volatile StatusLump globalStatus = {
    .ECUState = GLV_ON,
    .StatusBits = {0},
    .PowerLevelTorqueMap = POWERLEVEL_TORQUEMAP_RESET,
    .MaxCellTemp = 0x0,
    .AccumulatorStateOfCharge = 0x0,
    .GLVStateOfCharge = 0x0,
};

volatile uint8_t numberOfBadMessages = 0;
int32_t dischargeStartMillis = -1;
uint16_t lastECUStatusMsgTick = 0;

static const uint16_t howOftenToSpamECUStatusMsgs = 250;

void stateMachineTick(void)
{
    if (numberOfBadMessages >= 3)   // Magic value, 3 is bad arbitrarily (prime)
    {
        numberOfBadMessages = 0;
        Error_Handler();
    }

    if (globalStatus.ECUState != TS_DISCHARGE_OFF)
    {
        dischargeStartMillis = -1;
    }

    switch(globalStatus.ECUState)
    {
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

    if (HAL_GetTick() - lastECUStatusMsgTick > howOftenToSpamECUStatusMsgs)
    {
        writeMessage(PrimaryBusCAN, MSG_ECU_STATUS_1, GR_ALL, (uint8_t*)globalStatus.ECUStatusMsgOne, 8);
        writeMessage(PrimaryBusCAN, MSG_ECU_STATUS_2, GR_ALL, (uint8_t*)globalStatus.ECUStatusMsgTwo, 8);
        writeMessage(PrimaryBusCAN, MSG_ECU_STATUS_3, GR_ALL, (uint8_t*)globalStatus.ECUStatusMsgThree, 4);

        lastECUStatusMsgTick = HAL_GetTick();
    }
}

void glv_on(void)
{
    // For safety
    if (globalStatus.TractiveSystemVoltage >= TS_VOLTAGE_OFF_LIMIT)
    {
        globalStatus.ECUState = TS_DISCHARGE_OFF;
        return;
    }

    // Close software latch, should be error free at this point, also reset power level
    setSoftwareLatch(1);
    globalStatus.PowerLevelTorqueMap = POWERLEVEL_TORQUEMAP_RESET;
    // TS on handled in CANdler -> MSG_DASH_STATUS
}

void precharge_engaged(void)
{
    // For safety
    if (globalStatus.TractiveSystemVoltage >= TS_VOLTAGE_OFF_LIMIT)
    {
        globalStatus.ECUState = TS_DISCHARGE_OFF;
    }

    // ACU confirmation is IR-, handled in CANdler
    //TS ACTIVE botton disabled --> GLV_ON is handled in CANdler
}

void precharging(void)
{
    //TS ACTIVE button disabled || ACU precharge cancellation --> TS_DISCHARGE_OFF is handled in CANdler line 66, 176
}

void precharge_complete(void)
{
    // If front, rear, and rtd, then go to DRIVE_STANDBY handled in CANdler
    // TS ACTIVE, ACU shutdown, errors handled in CANdler
}


void ts_discharge_off(void)
{
    setSoftwareLatch(0);
    controlInverters(0);

    if (dischargeStartMillis == -1)
    {
        dischargeStartMillis = millis();
    }
    
    if ((millis() - dischargeStartMillis) > HOW_LONG_TSDISCHARGE_UNTIL_ERROR_MS)
    {
        globalStatus.ECUState = ERRORSTATE;  // ERRORSTATE will send it back if voltage >= TS_VOLTAGE_OFF_LIMIT
    }
}

void reflash_tune(void) // FIXME Currently a stub
{
    globalStatus.ECUState = GLV_ON;
    return;

    // READ SD CARD INFORMATION INTO INFO

    // if (true /*Flash error*/)
    // {
    //     globalStatus.ECUState = ERRORSTATE;
    // }
}

void error(void)
{
    setSoftwareLatch(0);
    controlInverters(0);

    if (globalStatus.TractiveSystemVoltage >= TS_VOLTAGE_OFF_LIMIT)
    {
        globalStatus.ECUState = TS_DISCHARGE_OFF;
    }

    /* Only error resolved when MSG_ACU AND GRI says we are good -> Handled in CANdler */
}
