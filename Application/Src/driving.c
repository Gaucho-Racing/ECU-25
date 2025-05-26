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

float vehicleSpeedMPH(void)
{
    return ((globalInverterData.msgOne.erpm / MOTOR_POLE_PAIRS) * 2 * M_PI * WHEEL_RADIUS_IN) / (GEAR_RATIO * 1056.0);  // TODO: Where did 1056 come from?
}

void sendBseAppsViolationMessage(void)
{
    uint8_t errorMap = 0x1;
    writeMessage(PrimaryBusCAN, MSG_DASH_WARNING_FLAGS, GR_DASH_PANEL, &errorMap, 1);
}

void drive_standby(void)
{
    controlInverters(true);

    if (!BSE_APPS_violation && (float)analogRead(APPS1_SIGNAL)/ADC_MAX >= APPS_DEADZONE)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
    }
}

void drive_active_idle(void)
{
    controlInverters(true);

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
    else if (throttle1 < APPS_DEADZONE && vehicleSpeedMPH() > REGEN_MPH && getBits(globalSteeringStatusRegenButtonMap, 0, 4) != 0)
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
    else if (throttle1 < APPS_DEADZONE)
    {
        globalStatus.ECUState = DRIVE_STANDBY;
    }
    else if (fabs(throttle1 - throttle2) > 0.1)
    {
        globalStatus.ECUState = DRIVE_STANDBY;
    }
    
    sendInverterCommand();

    //FIXME in ECU, dti stuff here
    uint8_t driveEnable = 1;

    uint16_t throttleMin = 15;
    uint16_t throttleMax = 100;
    uint16_t maxCurrentValue = 10; // 10 A
    uint16_t throttleRequest = (1 - (analogRead(APPS1_SIGNAL) - throttleMin) / ((double)(throttleMax - throttleMin))) * maxCurrentValue;

    // Scale throttle request for CAN messaging
    throttleRequest = (throttleRequest * 10) << 8; //(max - min) * request/0xFFFF ()

    writeDtiMessage(MSG_DTI_CONTROL_12, &driveEnable, 1);            // 1 Drive Enable

    writeDtiMessage(MSG_DTI_CONTROL_5, (uint8_t*) &throttleRequest, 2);
}

void drive_active_regen(void)
{
    if (BSE_APPS_violation)
    {
        globalStatus.ECUState = DRIVE_STANDBY;
        sendBseAppsViolationMessage();
    }
    else if ((float)analogRead(APPS1_SIGNAL) / ADC_MAX >= APPS_DEADZONE)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
    }
    else if (vehicleSpeedMPH() < REGEN_MPH || getBits(globalSteeringStatusRegenButtonMap, 0, 4) == 0)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_IDLE;
    }

    sendInverterCommand();
}
