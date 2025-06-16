#include <stdbool.h>
#include <math.h>

#include "driving.h"
#include "customIDs.h"
#include "CANdler.h"
#include "stateMachine.h"
#include "main.h"
#include "adc.h"
#include "inverter.h"
#include "fdcan.h"
#include "msgIDs.h"
#include "utils.h"

// TODO Confirm DTI read message callback works correctly (check endianness of bytes, bits are fine)
// TODO Mark all constants as floats so they do not become doubles

volatile bool BSE_APPS_violation = false;

volatile int32_t lastDriveActiveCtrlMs = BAD_TIME_Negative1;

volatile uint16_t heatCapacity1 = 0;

volatile uint16_t heatCapacity2 = 0;

volatile float minAmkHeatCapThrottlePercent = 0.8f;

static uint16_t getRegenPercent() // THIS IS NOT ACTUALLY BRAKE TRAVEL, PRESSURE SENSORS CAPTURE BRAKE TRAVEL
{
    globalStatus.AUX_SIGNAL = analogRead(AUX_SIGNAL);           // Brake pedal force (BSE_SIGNAL is a useless float)
    globalStatus.BRAKE_F_SIGNAL = analogRead(BRAKE_F_SIGNAL);   // Brake pressure F
    globalStatus.BRAKE_R_SIGNAL = analogRead(BRAKE_R_SIGNAL);   // Brake pressure R
    return (float)(globalStatus.AUX_SIGNAL - AUX_MIN) / (AUX_MAX - AUX_MIN);
}

static float getPedalTravel()
{
    globalStatus.APPS1_SIGNAL = analogRead(APPS1_SIGNAL);
    globalStatus.APPS2_SIGNAL = analogRead(APPS2_SIGNAL);
    return (float)(globalStatus.APPS1_SIGNAL + globalStatus.APPS2_SIGNAL - THROTTLE_MIN_2 - THROTTLE_MIN_1) / (THROTTLE_MAX_1 + THROTTLE_MAX_2 - THROTTLE_MIN_1 - THROTTLE_MIN_2);
}

void drive_standby(void)
{
    float pedalTravel = getPedalTravel();

    //escape condition for BSE_APPS_violation according to rules
    if (BSE_APPS_violation && pedalTravel < APPS_DEADZONE)
    {
        LOGOMATIC("Recovered from BSE APPS violation\n");
        BSE_APPS_violation = false;
    }

    if (!BSE_APPS_violation && pedalTravel >= APPS_DEADZONE)
    {
        LOGOMATIC("Moving to drive active with a torque request\n");
        controlInverters(true);
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
    }
}

void drive_active_idle(void)
{
    if (millis() - lastDriveActiveCtrlMs < DRIVE_ACTIVE_POWER_REGEN_INTERVAL_MS)
    {
        return;
    }

    lastDriveActiveCtrlMs = millis();
    float regenPercent = getRegenPercent();
    float pedalTravel = getPedalTravel();

    if (checkBSEAPPSviolation(globalStatus.APPS1_SIGNAL, globalStatus.APPS2_SIGNAL, pedalTravel, regenPercent))
    {
        LOGOMATIC("BSE APPS violation, pulling back to drive standby\n");
        controlInverters(false);   //false for disable
        globalStatus.ECUState = DRIVE_STANDBY;
        BSE_APPS_violation = true;
        sendBseAppsViolationMessage();
        return;
    }
    else if (pedalTravel >= APPS_DEADZONE)
    {
        LOGOMATIC("Torque request recieved, moving to drive active power\n");
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
        return;
    }
    else if (pedalTravel < APPS_DEADZONE && vehicleSpeedMPH() > REGEN_MPH && getBits(globalSteeringStatusRegenButtonMap, 0, 4) != 0)
    {
        LOGOMATIC("Moving to regen state\n");
        globalStatus.ECUState = DRIVE_ACTIVE_REGEN;
        return;
    }

    globalInverterSettings[0].acCurrent = 0;
    globalInverterSettings[1].acCurrent = 0;
    globalInverterSettings[2].acCurrent = 0;

    sendInverterCommand();

}

void drive_active_power(void)
{
    if (millis() - lastDriveActiveCtrlMs < DRIVE_ACTIVE_POWER_REGEN_INTERVAL_MS)
    {
        return;
    }

    lastDriveActiveCtrlMs = millis();
    float regenPercent = getRegenPercent();
    float pedalTravel = getPedalTravel();

    if (checkBSEAPPSviolation(globalStatus.APPS1_SIGNAL, globalStatus.APPS2_SIGNAL, pedalTravel, regenPercent))
    {
        LOGOMATIC("BSE APPS violation, pulling back to drive standby\n");
        controlInverters(false);   //0 for disable
        globalStatus.ECUState = DRIVE_STANDBY;
        BSE_APPS_violation = true;
        sendBseAppsViolationMessage();
        return;
    }
    else if (pedalTravel < APPS_DEADZONE)
    {
        LOGOMATIC("No torque request moving back to idle\n");
        globalStatus.ECUState = DRIVE_ACTIVE_IDLE;
        return;
    }

    // Scale throttle request for CAN messaging

    //Owen said that current after 5% should start from 0, hence the following line, but the car might not even move from 0-5% current, so maybe review the following line later

    uint16_t rearThrottleRequest = (uint16_t)((pedalTravel - 0.05f) / 0.95f * MAX_CURRENT_REAR);

    #ifdef ENABLE_THREE_MOTORS
    uint16_t forwardThrottleRequest1 = (uint16_t)((pedalTravel - 0.05f) / 0.95f * MAX_CURRENT_FORWARD);
    uint16_t forwardThrottleRequest2 = (uint16_t)((pedalTravel - 0.05f) / 0.95f * MAX_CURRENT_FORWARD);
    validateForwardTorqueRequest(&forwardThrottleRequest1, &heatCapacity1);
    validateForwardTorqueRequest(&forwardThrottleRequest2, &heatCapacity2);
    #endif

    //TODO: ADD MAX HEAT CAP ADJUSTMENT BASED ON COOLANT
    //TODO Wait, how to handle one forward motor overheating but the other one being fine? Does the other one also get limited?

    rearThrottleRequest *= 10;
    forwardThrottleRequest1 = (forwardThrottleRequest1 + 327.69f) * 100;
    forwardThrottleRequest2 = (forwardThrottleRequest2 + 327.69f) * 100;

    globalInverterSettings[0].acCurrent = rearThrottleRequest;

    #ifdef ENABLE_THREE_MOTORS
    globalInverterSettings[1].acCurrent = forwardThrottleRequest1;
    globalInverterSettings[2].acCurrent = forwardThrottleRequest2;
    #endif

    sendInverterCommand();
}

void drive_active_regen(void)
{
    if (millis() - lastDriveActiveCtrlMs < DRIVE_ACTIVE_POWER_REGEN_INTERVAL_MS)
    {
        return;
    }

    lastDriveActiveCtrlMs = millis();
    float regenPercent = getRegenPercent();
    float pedalTravel = getPedalTravel();

    if (checkBSEAPPSviolation(globalStatus.APPS1_SIGNAL, globalStatus.APPS2_SIGNAL, pedalTravel, regenPercent))
    {
        LOGOMATIC("BSE APPS violation, pulling back to drive standby\n");
        controlInverters(false);
        globalStatus.ECUState = DRIVE_STANDBY;
        BSE_APPS_violation = true;
        sendBseAppsViolationMessage();
        return;
    }
    else if (pedalTravel >= APPS_DEADZONE)
    {
        LOGOMATIC("Torque request recieved, moving to active power\n");
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
        return;
    }
    else if (vehicleSpeedMPH() < REGEN_MPH || getBits(globalSteeringStatusRegenButtonMap, 0, 4) == 0)
    {
        LOGOMATIC("Nothing much going down, moving to idle\n");
        globalStatus.ECUState = DRIVE_ACTIVE_IDLE;
        return;
    }

    uint16_t rearThrottleRequest = (uint16_t)(regenPercent * MAX_CURRENT_REAR);

    #ifdef ENABLE_THREE_MOTORS
    uint16_t forwardThrottleRequest1 = (uint16_t)(regenPercent * MAX_CURRENT_FORWARD);
    uint16_t forwardThrottleRequest2 = (uint16_t)(regenPercent * MAX_CURRENT_FORWARD);
    validateForwardTorqueRequest(&forwardThrottleRequest1, &heatCapacity1);
    validateForwardTorqueRequest(&forwardThrottleRequest2, &heatCapacity2);
    #else
    uint16_t forwardThrottleRequest1 = 0;
    uint16_t forwardThrottleRequest2 = 0;
    #endif

    validateRegenRequest(&rearThrottleRequest, &forwardThrottleRequest1, &forwardThrottleRequest2);
    
    //I'm assuming that reverse current heat management applies equally to all motors since it is for the battery.

    rearThrottleRequest *= -10;
    forwardThrottleRequest1 = (-1 * forwardThrottleRequest1 + 327.69f) * 100;
    forwardThrottleRequest2 = (-1 * forwardThrottleRequest2 + 327.69f) * 100;

    globalInverterSettings[0].acCurrent = rearThrottleRequest;

    #ifdef ENABLE_THREE_MOTORS
    globalInverterSettings[1].acCurrent = forwardThrottleRequest1;
    globalInverterSettings[2].acCurrent = forwardThrottleRequest2;
    #endif

    sendInverterCommand();
}
