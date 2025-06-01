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

volatile bool BSE_APPS_violation = false;

//volatile uint16_t heatCapacity = 0;
//volatile int32_t lastHeatCapacityUpdateMillis = BAD_TIME_Negative1;


static float getThrottle1()
{
    return analogRead(APPS1_SIGNAL) * ADC_CONV;
}

static float getThrottle2()
{
    return analogRead(APPS2_SIGNAL) * ADC_CONV;
}

static float getBrakeTravel()
{
    // TODO Check which signal
    return (analogRead(BSE_SIGNAL) * ADC_CONV - BRAKE_MIN) / (BRAKE_MAX - BRAKE_MIN);
}

static float getPedalTravel()
{
    return (getThrottle2() + getThrottle1() - THROTTLE_MIN_2 - THROTTLE_MIN_1) / (THROTTLE_MAX_1 + THROTTLE_MAX_2 - THROTTLE_MIN_1 - THROTTLE_MIN_2);
}

void drive_standby(void)
{
    controlInverters(true);

    float pedalTravel = getPedalTravel();

    //escape condition for BSE_APPS_violation according to rules
    if(BSE_APPS_violation && pedalTravel < APPS_DEADZONE){
        BSE_APPS_violation = false;
    }

    if (!BSE_APPS_violation && pedalTravel >= APPS_DEADZONE)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
    }
}

void drive_active_idle(void)
{
    controlInverters(true);

    float brakeTravel = getBrakeTravel();
    float pedalTravel = getPedalTravel();

    if (checkBSEAPPSviolation(getThrottle1(), getThrottle2(), pedalTravel, brakeTravel))
    {  
        controlInverters(0);   //0 for disable
        globalStatus.ECUState = DRIVE_STANDBY;
        BSE_APPS_violation = true;
        sendBseAppsViolationMessage();
        return;
    }
    else if (pedalTravel >= APPS_DEADZONE)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
    }
    else if (pedalTravel < APPS_DEADZONE && vehicleSpeedMPH() > REGEN_MPH && getBits(globalSteeringStatusRegenButtonMap, 0, 4) != 0)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_REGEN;
    }
}

void drive_active_power(void)
{
    float brakeTravel = getBrakeTravel();
    float pedalTravel = getPedalTravel();

    if (checkBSEAPPSviolation(getThrottle1(), getThrottle2(), pedalTravel, brakeTravel)){
        controlInverters(0);   //0 for disable
        globalStatus.ECUState = DRIVE_STANDBY;
        BSE_APPS_violation = true;
        sendBseAppsViolationMessage();
        return;
    }
    else if (pedalTravel < APPS_DEADZONE)
    {
        globalStatus.ECUState = DRIVE_STANDBY;
    }

    // Scale throttle request for CAN messaging

    uint16_t rearThrottleRequest = (uint16_t)(pedalTravel * MAX_CURRENT_REAR * 10) << 8;
    uint16_t forwardThrottleRequest = (uint16_t)(pedalTravel * MAX_CURRENT_FORWARD * 10) << 8;

    //validateForwardTorqueRequest((int16_t*)&forwardThrottleRequest);

    globalInverterSettings[0].acCurrent = rearThrottleRequest;
    globalInverterSettings[1].acCurrent = forwardThrottleRequest;
    globalInverterSettings[2].acCurrent = forwardThrottleRequest;

    sendInverterCommand();
}

void drive_active_regen(void)
{
    float brakeTravel = getBrakeTravel();
    float pedalTravel = getPedalTravel();

    if(checkBSEAPPSviolation(getThrottle1(), getThrottle2(), pedalTravel, brakeTravel))
    {
        controlInverters(0);
        globalStatus.ECUState = DRIVE_STANDBY;
        BSE_APPS_violation = true;
        sendBseAppsViolationMessage();
        return;
    }
    else if (getPedalTravel() >= APPS_DEADZONE)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
    }
    else if (vehicleSpeedMPH() < REGEN_MPH || getBits(globalSteeringStatusRegenButtonMap, 0, 4) == 0)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_IDLE;
    }

    int16_t rearTorqueRequest = (int16_t)(getBrakeTravel() * MAX_CURRENT_REAR * -10) << 8;
    int16_t forwardTorqueRequest = (int16_t)(getPedalTravel() * MAX_CURRENT_FORWARD * 10) << 8;

    //validateForwardTorqueRequest(&forwardTorqueRequest);

    globalInverterSettings[0].acCurrent = rearTorqueRequest;
    globalInverterSettings[1].acCurrent = forwardTorqueRequest;
    globalInverterSettings[2].acCurrent = forwardTorqueRequest;

    sendInverterCommand();
}
