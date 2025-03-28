#include <stdbool.h>
#include <math.h>

#include "stateMachine.h"
#include "driving.h"
#include "main.h"
#include "adc.h"
#include "inverter.h"
#include "fdcan.h"
#include "msgIDs.h"
#include "utils.h"

volatile bool BSE_APPS_violation = false;

float vehicleSpeedMPH(void)
{
    return ((getERPM() / MOTOR_POLE_PAIRS) * 2 * M_PI * WHEEL_RADIUS_IN) / (GEAR_RATIO * 1056.0);
}

void sendBseAppsViolationMessage(void)
{
    uint8_t errorMap = 0x01;
    writeMessage(1, MSG_DASH_WARNING_FLAGS, GR_DASH_PANEL, &errorMap, 1);
}

void drive_standby(void)
{
    controlInverters(1);

    if (!BSE_APPS_violation && (float)analogRead(APPS1_SIGNAL)/ADC_MAX >= APPS_DEADZONE)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
    }
}

void drive_active_idle(void)
{
    controlInverters(1);

    float throttle1 = (float)analogRead(APPS1_SIGNAL)/ADC_MAX;

    if (BSE_APPS_violation)
    {  
        globalStatus.ECUState = DRIVE_STANDBY;
        sendBseAppsViolationMessage();
    }
    else if (throttle1 >= APPS_DEADZONE)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
    }
    else if (throttle1 < APPS_DEADZONE && vehicleSpeedMPH() > REGEN_MPH && globalSteeringStatusRegen > 0)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_REGEN;
    }
}

void drive_active_power(void)
{
    float throttle1 = (float)analogRead(APPS1_SIGNAL) / ADC_MAX;
    float throttle2 = (float)analogRead(APPS2_SIGNAL) / ADC_MAX;
    float brake = (float)analogRead(BSE_SIGNAL) / ADC_MAX;

    if (BSE_APPS_violation)
    {
        globalStatus.ECUState = DRIVE_STANDBY;
        sendBseAppsViolationMessage();
    }
    else if (brake >= BSE_DEADZONE && throttle1 >= 0.25)
    {
        globalStatus.ECUState = DRIVE_STANDBY;
        BSE_APPS_violation = true;
    }
    else if(throttle1 < APPS_DEADZONE)
    {
        globalStatus.ECUState = DRIVE_STANDBY;
    }
    else if (fabs(throttle1 - throttle2) > 0.1)
    {
        globalStatus.ECUState = DRIVE_STANDBY;
    }

    sendInverterCommand();
}

void drive_active_regen(void)
{
    if (BSE_APPS_violation)
    {
        globalStatus.ECUState = DRIVE_STANDBY;
        sendBseAppsViolationMessage();
    }
    else if ((float)analogRead(APPS1_SIGNAL)/ADC_MAX >= APPS_DEADZONE)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
    }
    else if(vehicleSpeedMPH() < REGEN_MPH || globalSteeringStatusRegen == 0)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_IDLE;
    }

    sendInverterCommand();
}
