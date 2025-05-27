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

// TODO Copy over from dtiMotorTest the throttle setup

volatile bool BSE_APPS_violation = false;

static const uint8_t driveDisable = 0;
static const uint8_t driveEnable = 1;

void drive_standby(void)
{
    controlInverters(true);

    float throttle2 = (float) analogRead(APPS2_SIGNAL) / (ADC_MAX);
    float pedalTravel = (throttle2 - THROTTLE_MIN_2) / (THROTTLE_MAX_2 - THROTTLE_MIN_2);

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

    float throttle1 = analogRead(APPS1_SIGNAL) * ADC_CONV;
    float throttle2 = analogRead(APPS2_SIGNAL) * ADC_CONV;
    float brakeTravel = (analogRead(BSE_SIGNAL) * ADC_CONV - BRAKE_MIN) / (BRAKE_MAX - BRAKE_MIN);
    float pedalTravel = (throttle2 - THROTTLE_MIN_2) / (THROTTLE_MAX_2 - THROTTLE_MIN_2);

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
    float pedalTravel = (throttle2 - THROTTLE_MIN_2) / (THROTTLE_MAX_2 - THROTTLE_MIN_2);

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

    // Scale throttle request for CAN messaging

    uint16_t rearThrottleRequest = (uint16_t)(pedalTravel * MAX_CURRENT_REAR * 10) << 8;
    uint16_t forwardThrottleRequest = (uint16_t)(pedalTravel * MAX_CURRENT_FORWARD * 10) << 8;

    //Assuming dti inverter is #0
    globalInverterSettings[0].Set_AC_Current = rearThrottleRequest;
    
    sendInverterCommand();

    writeDtiMessage(MSG_DTI_CONTROL_12, (uint8_t*)&driveEnable, 1);            // 1 Drive Enable

    writeDtiMessage(MSG_DTI_CONTROL_5, (uint8_t*)&rearThrottleRequest, 2);

    //TODO: Figure out how and where to sed forward throttle request. Is it just the other two inverters?
}

void drive_active_regen(void)
{
    float throttle1 = analogRead(APPS1_SIGNAL) * ADC_CONV;
    float throttle2 = analogRead(APPS2_SIGNAL) * ADC_CONV;
    float brakeTravel = (analogRead(BSE_SIGNAL) * ADC_CONV - BRAKE_MIN) / (BRAKE_MAX - BRAKE_MIN);
    float pedalTravel = (throttle2 - THROTTLE_MIN_2) / (THROTTLE_MAX_2 - THROTTLE_MIN_2);

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

    //TODO: Figure out wtf below line is supposed to be
    //sendInverterCommand();

    uint16_t brakerequest = (uint16_t)(brakeTravel * MAX_CURRENT_REAR * 10) << 8;

    writeDtiMessage(MSG_DTI_CONTROL_6, (uint8_t*)&brakerequest, 2);

}
