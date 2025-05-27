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

static const uint8_t driveDisable = 0;
static const uint8_t driveEnable = 1;

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

    float throttle2 = (float) analogRead(APPS2_SIGNAL) * ADC_CONV;
    float pedalTravel = (throttle2 - THROTTLE_MIN_2) / (THROTTLE_MAX_2 - THROTTLE_MIN_2);

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

    float throttle1 = analogRead(APPS1_SIGNAL) * ADC_CONV;
    float throttle2 = analogRead(APPS2_SIGNAL) * ADC_CONV;
    float brakeTravel = (analogRead(BSE_SIGNAL) * ADC_CONV - BRAKE_MIN) / (BRAKE_MAX - BRAKE_MIN);
    float pedalTravel = (throttle1 - THROTTLE_MIN_1) / (THROTTLE_MAX_1 - THROTTLE_MIN_1);
    //float pedalTravel = (throttle2 - THROTTLE_MIN_2) / (THROTTLE_MAX_2 - THROTTLE_MIN_2);

    if (checkBSEAPPSviolation(throttle1, throttle2, pedalTravel, brakeTravel))
    {  
        writeDtiMessage(MSG_DTI_CONTROL_12, (uint8_t*)&driveDisable, 1);   //0 for disable
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
    float throttle1 = analogRead(APPS1_SIGNAL) * ADC_CONV;
    float throttle2 = analogRead(APPS2_SIGNAL) * ADC_CONV;
    float brakeTravel = (analogRead(BSE_SIGNAL) * ADC_CONV - BRAKE_MIN) / (BRAKE_MAX - BRAKE_MIN);
    float pedalTravel = (throttle1 - THROTTLE_MIN_1) / (THROTTLE_MAX_1 - THROTTLE_MIN_1);

    if (checkBSEAPPSviolation(throttle1, throttle2, pedalTravel, brakeTravel)){
        writeDtiMessage(MSG_DTI_CONTROL_12, (uint8_t*)&driveDisable, 1);   //0 for disable
        globalStatus.ECUState = DRIVE_STANDBY;
        BSE_APPS_violation = true;
        sendBseAppsViolationMessage();
        return;
    }
    else if (pedalTravel < APPS_DEADZONE)
    {
        globalStatus.ECUState = DRIVE_STANDBY;
    }
    
    sendInverterCommand();

    // Scale throttle request for CAN messaging

    uint16_t throttleRequest = (uint16_t)(pedalTravel * MAX_CURRENT * 10) << 8;

    writeDtiMessage(MSG_DTI_CONTROL_12, (uint8_t*)&driveEnable, 1);            // 1 Drive Enable

    writeDtiMessage(MSG_DTI_CONTROL_5, (uint8_t*)&throttleRequest, 2);
}

void drive_active_regen(void)
{
    float throttle1 = analogRead(APPS1_SIGNAL) * ADC_CONV;
    float throttle2 = analogRead(APPS2_SIGNAL) * ADC_CONV;
    float brakeTravel = (analogRead(BSE_SIGNAL) * ADC_CONV - BRAKE_MIN) / (BRAKE_MAX - BRAKE_MIN);
    float pedalTravel = (throttle1 - THROTTLE_MIN_1) / (THROTTLE_MAX_1 - THROTTLE_MIN_1);
    //float pedalTravel = (throttle2 - THROTTLE_MIN_2) / (THROTTLE_MAX_2 - THROTTLE_MIN_2);

    if(checkBSEAPPSviolation(throttle1, throttle2, pedalTravel, brakeTravel)){
        writeDtiMessage(MSG_DTI_CONTROL_12, (uint8_t*)&driveDisable, 1);   //0 for disable
        globalStatus.ECUState = DRIVE_STANDBY;
        BSE_APPS_violation = true;
        sendBseAppsViolationMessage();
        return;
    }
    else if (pedalTravel >= APPS_DEADZONE)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
    }
    else if (vehicleSpeedMPH() < REGEN_MPH || getBits(globalSteeringStatusRegenButtonMap, 0, 4) == 0)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_IDLE;
    }

    sendInverterCommand();
}
