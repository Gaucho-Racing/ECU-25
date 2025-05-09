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
int32_t dischargeStartMillis = BAD_TIME_Negative1;
uint16_t lastECUStatusMsgTick = 0;

static const uint16_t howOftenToSpamECUStatusMsgs = 250;    // Might change

void stateMachineTick(void)
{
    if (numberOfBadMessages > BAD_MESSAGE_LIMIT)
    {
        Error_Handler();
    }

    if (globalStatus.ECUState != TS_DISCHARGE_OFF)
    {
        dischargeStartMillis = BAD_TIME_Negative1;
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

    StatusLump correctlyScaledValues = scaledECUStatusMsgForTx();

    if (HAL_GetTick() - lastECUStatusMsgTick > howOftenToSpamECUStatusMsgs)
    {
        writeMessage(PrimaryBusCAN, MSG_ECU_STATUS_1, GR_ALL, (uint8_t*)correctlyScaledValues.ECUStatusMsgOne, 8);
        writeMessage(PrimaryBusCAN, MSG_ECU_STATUS_2, GR_ALL, (uint8_t*)correctlyScaledValues.ECUStatusMsgTwo, 8);
        writeMessage(PrimaryBusCAN, MSG_ECU_STATUS_3, GR_ALL, (uint8_t*)correctlyScaledValues.ECUStatusMsgThree, 4);

        lastECUStatusMsgTick = HAL_GetTick();

        LOGOMATIC("\n--Global Status Dump--\nECU State %d\n", correctlyScaledValues.ECUState);
        LOGOMATIC("Status Bits 1 %X%X%x\n", correctlyScaledValues.StatusBits[0], correctlyScaledValues.StatusBits[1], correctlyScaledValues.StatusBits[2]);
        LOGOMATIC("Power Level & Torque Map %X\n", correctlyScaledValues.PowerLevelTorqueMap);
        LOGOMATIC("Max Cell Temp %d\n", correctlyScaledValues.MaxCellTemp);
        LOGOMATIC("Accumulator SoC %d\n", correctlyScaledValues.AccumulatorStateOfCharge);
        LOGOMATIC("GLV SoC %d\n", correctlyScaledValues.GLVStateOfCharge);
        LOGOMATIC("TS Voltage %d\n", correctlyScaledValues.TractiveSystemVoltage);
        LOGOMATIC("Vehicle Speed %d\n", correctlyScaledValues.VehicleSpeed);
        LOGOMATIC("FR Wheel RPM %d\n", correctlyScaledValues.FRWheelRPM);
        LOGOMATIC("FL Wheel RPM %d\n", correctlyScaledValues.FLWheelRPM);
        LOGOMATIC("RR Wheel RPM %d\n", correctlyScaledValues.RRWheelRPM);
        LOGOMATIC("RL Wheel RPM %d\n", correctlyScaledValues.RLWheelRPM);
        LOGOMATIC("--Global Status Dump--\n");

        uint8_t onlinePingBits = 0;
        for (uint8_t i = 0; i < 3; i++)
        {
            for (uint8_t j = 0; j < 8; j++)
            {
                if ((correctlyScaledValues.StatusBits[i] >> j) & 0x1)
                {
                    onlinePingBits++;
                }
            }
        }
        writeMessage(DataBusCAN, MSG_ECU_PING_INFORMATION, GR_STEERING_WHEEL, &onlinePingBits, 1);
    }
}

StatusLump scaledECUStatusMsgForTx(void)
{
    StatusLump scaledStatus = globalStatus;

    scaledStatus.MaxCellTemp *= 4;
    scaledStatus.AccumulatorStateOfCharge = (uint8_t)(scaledStatus.AccumulatorStateOfCharge * 51.0 / 20.0);
    scaledStatus.GLVStateOfCharge *= (uint8_t)(scaledStatus.GLVStateOfCharge * 51.0 / 20.0);
    scaledStatus.TractiveSystemVoltage *= 100;
    scaledStatus.VehicleSpeed *= 100;
    scaledStatus.FLWheelRPM = scaledStatus.FLWheelRPM * 10 + 32768;
    scaledStatus.FRWheelRPM = scaledStatus.FRWheelRPM * 10 + 32768;
    scaledStatus.RLWheelRPM = scaledStatus.RLWheelRPM * 10 + 32768;
    scaledStatus.RRWheelRPM = scaledStatus.RRWheelRPM * 10 + 32768;

    return scaledStatus;
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
    setSoftwareLatch(true);
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
    setSoftwareLatch(false);
    controlInverters(false);

    if (dischargeStartMillis == BAD_TIME_Negative1)
    {
        dischargeStartMillis = millis();
    }
    
    if ((millis() - dischargeStartMillis) > HOW_LONG_TSDISCHARGE_UNTIL_ERROR_MS)
    {
        globalStatus.ECUState = ERRORSTATE;  // ERRORSTATE will send it back if voltage >= TS_VOLTAGE_OFF_LIMIT
    }
}

void reflash_tune(void) // TODO Currently a stub, may decide to use CAN for updating settings (on each boot)
{
    globalStatus.ECUState = GLV_ON;
    return;

    // Planned reading and parsing of SD card contents into settings

    // if (true /*Flash error*/)
    // {
    //     globalStatus.ECUState = ERRORSTATE;
    // }
}

void error(void)
{
    setSoftwareLatch(false);
    controlInverters(false);

    if (globalStatus.TractiveSystemVoltage >= TS_VOLTAGE_OFF_LIMIT)
    {
        globalStatus.ECUState = TS_DISCHARGE_OFF;
    }

    /* Only error resolved when MSG_ACU AND GRI says we are good -> Handled in CANdler */
}
