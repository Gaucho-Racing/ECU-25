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

volatile uint16_t batteryHeatCapacity = 0;

volatile float minAmkHeatCapThrottlePercent = 0.8f;

volatile float minBatHeatCapThrottlePercent = 0.8f;

static uint16_t getBrakeTravel()
{
    // TODO Check which signal
    globalStatus.BRAKE_FORCE = analogRead(/*Change this*/BSE_SIGNAL);
    globalStatus.BRAKE_PRESSURE = analogRead(/*Change this*/BSE_SIGNAL);
    return analogRead(BSE_SIGNAL);
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
    if(BSE_APPS_violation && pedalTravel < APPS_DEADZONE){
        BSE_APPS_violation = false;
    }

    if (!BSE_APPS_violation && pedalTravel >= APPS_DEADZONE)
    {
        controlInverters(true);
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
    }
}

void drive_active_idle(void)
{
    if(millis() - lastDriveActiveCtrlMs < DRIVE_ACTIVE_POWER_REGEN_INTERVAL_MS){
        return;
    }

    lastDriveActiveCtrlMs = millis();
    float brakeTravel = getBrakeTravel();
    float pedalTravel = getPedalTravel();

    if (checkBSEAPPSviolation(globalStatus.APPS1_SIGNAL, globalStatus.APPS2_SIGNAL, pedalTravel, brakeTravel))
    {  
        controlInverters(false);   //false for disable
        globalStatus.ECUState = DRIVE_STANDBY;
        BSE_APPS_violation = true;
        sendBseAppsViolationMessage();
        return;
    }
    else if (pedalTravel >= APPS_DEADZONE)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
        return;
    }
    else if (pedalTravel < APPS_DEADZONE && vehicleSpeedMPH() > REGEN_MPH && getBits(globalSteeringStatusRegenButtonMap, 0, 4) != 0)
    {
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
    float brakeTravel = getBrakeTravel();
    float pedalTravel = getPedalTravel();

    if (checkBSEAPPSviolation(globalStatus.APPS1_SIGNAL, globalStatus.APPS2_SIGNAL, pedalTravel, brakeTravel)){
        controlInverters(false);   //0 for disable
        globalStatus.ECUState = DRIVE_STANDBY;
        BSE_APPS_violation = true;
        sendBseAppsViolationMessage();
        return;
    }
    else if (pedalTravel < APPS_DEADZONE)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_IDLE;
        return;
    }

    // Scale throttle request for CAN messaging

    //Owen said that current after 5% should start from 0, hence the following line, but the car might not even move from 0-5% current, so maybe review the following line later

    uint16_t rearThrottleRequest = (uint16_t)((pedalTravel - 0.05f) / 0.95f * MAX_CURRENT_REAR);
    uint16_t forwardThrottleRequest1 = (uint16_t)((pedalTravel - 0.05f) / 0.95f * MAX_CURRENT_FORWARD); // 65535/655.35 = 100
    uint16_t forwardThrottleRequest2 = (uint16_t)((pedalTravel - 0.05f) / 0.95f * MAX_CURRENT_FORWARD); // 65535/655.35 = 100

    validateForwardTorqueRequest(&forwardThrottleRequest1, &heatCapacity1);
    validateForwardTorqueRequest(&forwardThrottleRequest2, &heatCapacity2);

    //TODO: ADD MAX HEAT CAP ADJUSTMENT BASED ON COOLANT
    //TODO Wait, how to handle one forward motor overheating but the other one being fine? Does the other one also get limited?


    //validateForwardTorqueRequest((int16_t*)&forwardThrottleRequest);

    rearThrottleRequest *= 10;
    forwardThrottleRequest1 = (forwardThrottleRequest1 + 327.69f) * 100;
    forwardThrottleRequest2 = (forwardThrottleRequest2 + 327.69f) * 100;

    globalInverterSettings[0].acCurrent = rearThrottleRequest;
    globalInverterSettings[1].acCurrent = forwardThrottleRequest1;
    globalInverterSettings[2].acCurrent = forwardThrottleRequest2;

    sendInverterCommand();
}

void drive_active_regen(void)
{
    if(millis() - lastDriveActiveCtrlMs < DRIVE_ACTIVE_POWER_REGEN_INTERVAL_MS){
        return;
    }
    lastDriveActiveCtrlMs = millis();
    float brakeTravel = getBrakeTravel();
    float pedalTravel = getPedalTravel();

    if(checkBSEAPPSviolation(globalStatus.APPS1_SIGNAL, globalStatus.APPS2_SIGNAL, pedalTravel, brakeTravel))
    {
        controlInverters(false);
        globalStatus.ECUState = DRIVE_STANDBY;
        BSE_APPS_violation = true;
        sendBseAppsViolationMessage();
        return;
    }
    else if (pedalTravel >= APPS_DEADZONE)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
        return;
    }
    else if (vehicleSpeedMPH() < REGEN_MPH || getBits(globalSteeringStatusRegenButtonMap, 0, 4) == 0)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_IDLE;
        return;
    }

    uint16_t rearThrottleRequest = (uint16_t)(brakeTravel * MAX_CURRENT_REAR);
    uint16_t forwardThrottleRequest1 = (uint16_t)(brakeTravel * MAX_CURRENT_FORWARD); // 65535/655.35 = 100
    uint16_t forwardThrottleRequest2 = (uint16_t)(brakeTravel * MAX_CURRENT_FORWARD); // 65535/655.35 = 100

    validateForwardTorqueRequest(&forwardThrottleRequest1, &heatCapacity1);
    validateForwardTorqueRequest(&forwardThrottleRequest2, &heatCapacity2);

    validateRegenRequest(&rearThrottleRequest, &forwardThrottleRequest1, &forwardThrottleRequest2, &batteryHeatCapacity);
    
    //I'm assuming that reverse current heat management applies equally to all motors since it is for the battery.

    //validateForwardTorqueRequest(&forwardTorqueRequest);

    rearThrottleRequest *= -10;
    forwardThrottleRequest1 = (-1 * forwardThrottleRequest1 + 327.69f) * 100;
    forwardThrottleRequest2 = (-1 * forwardThrottleRequest2 + 327.69f) * 100;

    globalInverterSettings[0].acCurrent = rearThrottleRequest;
    globalInverterSettings[1].acCurrent = forwardThrottleRequest1;
    globalInverterSettings[2].acCurrent = forwardThrottleRequest2;

    sendInverterCommand();
}
