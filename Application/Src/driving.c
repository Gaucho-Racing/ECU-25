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

bool BSE_APPS_violation = false;
volatile InverterLump globalInverterSettings = {0};

inline float mVehicleSpeedMPH()
{
    return ((getERPM() / MOTOR_POLE_PAIRS) * 2 * M_PI * WHEEL_RADIUS_IN) / (GEAR_RATIO * 1056.0);
}

int32_t lastInverterPingMillis = -1;

void sendInverterCommand()
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

void drive_standby(void)
{
    controlInverters(1);

    if (!BSE_APPS_violation && (float)analogRead(APPS1_SIGNAL)/ADC_MAX >= APPS_DEADZONE) // Valid torque request
    {
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
    }
}

void drive_active_idle(void)
{
    controlInverters(1);
    // LOTS OF https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1214
    float throttle1 = (float)analogRead(APPS1_SIGNAL)/ADC_MAX;

    if (throttle1 >= APPS_DEADZONE)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
    }
    else if (!BSE_APPS_violation && throttle1 < APPS_DEADZONE && mVehicleSpeedMPH() > REGEN_MPH && true /*regen config here*/)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_REGEN;
    }
    else if (BSE_APPS_violation)
    {  
        //TODO SEND WARNING TO DASH HERE
        globalStatus.ECUState = DRIVE_STANDBY;
    }
}

void drive_active_power(void)
{
    // LOTS OF https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1214
    float throttle1 = (float)analogRead(APPS1_SIGNAL)/ADC_MAX;
    float throttle2 = (float)analogRead(APPS2_SIGNAL)/ADC_MAX;
    float brake = (float)analogRead(BSE_SIGNAL)/ADC_MAX;


    if(throttle1 < APPS_DEADZONE)
    {
        globalStatus.ECUState = DRIVE_STANDBY;
    }
    else if (fabs(throttle1 - throttle2) > 0.1)
    {
        globalStatus.ECUState = DRIVE_STANDBY;
    }
    else if (brake >= BSE_DEADZONE && throttle1 >= 0.25)
    {
        globalStatus.ECUState = DRIVE_STANDBY;
    }

    sendInverterCommand();
}

void drive_active_regen(void)
{
    // LOTS OF https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1214
    // Some math in https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1253

    if (BSE_APPS_violation)
    {
        globalStatus.ECUState = DRIVE_STANDBY;
    }
    if ((float)analogRead(APPS1_SIGNAL)/ADC_MAX >= APPS_DEADZONE && )
    {
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
    }
    else if(mVehicleSpeedMPH() < REGEN_MPH && true /*regen config here*/)
    else if (false /*Settings say no regen braking*/)
    {
        globalStatus.ECUState = DRIVE_ACTIVE_IDLE;
    }
    sendInverterCommand();
}
