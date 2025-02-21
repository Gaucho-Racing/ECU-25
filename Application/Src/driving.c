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
volatile InverterLump globalInverterSettings = {0};

volatile int32_t lastInverterPingMillis = -1;

float mVehicleSpeedMPH(void)
{
    return ((getERPM() / MOTOR_POLE_PAIRS) * 2 * M_PI * WHEEL_RADIUS_IN) / (GEAR_RATIO * 1056.0);
}

void sendInverterCommand(void)
{
    if(millis() - lastInverterPingMillis >= 50) // Must send every 100 ms
    {
        lastInverterPingMillis = millis();
        writeMessage(1, MSG_INVERTER_COMMAND, GR_GR_INVERTER_1, (uint8_t*)(globalInverterSettings.firstMsg), 7);
        writeMessage(1, MSG_INVERTER_COMMAND, GR_GR_INVERTER_2, (uint8_t*)(globalInverterSettings.secondMsg), 7);
        writeMessage(1, MSG_INVERTER_COMMAND, GR_GR_INVERTER_3, (uint8_t*)(globalInverterSettings.thirdMsg), 7);
//      writeMessage(1, MSG_INVERTER_COMMAND, GR_GR_INVERTER_4, (uint8_t*)(globalInverterSettings.fourthMsg), 7);   // Enable iff fourth motor/inverter
    }
}

void sendBseAppsViolationMessage(void)
{
    // SEND WARNING TO DASH :)
    // PENDING WHAT FORMAT THEY WANT
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
    else if (throttle1 < APPS_DEADZONE && mVehicleSpeedMPH() > REGEN_MPH && globalSteeringStatusRegen > 0)
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
    else if(mVehicleSpeedMPH() < REGEN_MPH || globalSteeringStatusRegen == 0)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_IDLE;
    }

    sendInverterCommand();
}
