#include <stdbool.h>
#include <math.h>

#include "stateMachine.h"
#include "driving.h"
#include "main.h"
#include "adc.h"
#include "inverter.h"

bool BSE_APPS_violation = false;

inline float mVehicleSpeedMPH(){return ((getERPM()/MOTOR_POLE_PAIRS)*2*PI*WHEEL_RADIUS_IN)/(GEAR_RATIO*1056.0);}

void drive_standby(void)
{
/*
    // If not rtd, then go back to precharge_complete
    if (!HAL_GPIO_ReadPin(RTD_CONTROL_GPIO_Port, RTD_CONTROL_Pin))
        globalStatus.ECUState = PRECHARGE_COMPLETE;
    if(false Valid torque request || false APPS stuff)
        globalStatus.ECUState = DRIVE_ACTIVE_IDLE;
    // TS ACTIVE, ACU shutdown, errors handled in CANdler.c
*/
    // If rtd off, go back to precharge complete handled in CAN
    
    // torque stuff waiting for vdm
}

void drive_active_idle(void)
{
    // LOTS OF https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1214
    float throttle1 = (float)analogRead(APPS1_SIGNAL)/ADC_MAX;

    if (throttle1 >= APPS_DEADZONE) {
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
        return;
    }
    if (!BSE_APPS_violation && throttle1 < APPS_DEADZONE && mVehicleSpeedMPH() > REGEN_MPH && true /*regen config here*/) {
        globalStatus.ECUState = DRIVE_ACTIVE_REGEN;
        return;
    }
    if (BSE_APPS_violation) {   // TODO: SEND WARNING TO DASH HERE
       globalStatus.ECUState = DRIVE_STANDBY;
       return;
    }
}

void drive_active_power(void)
{
    // LOTS OF https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1214
    float throttle1 = (float)analogRead(APPS1_SIGNAL)/ADC_MAX;
    float throttle2 = (float)analogRead(APPS2_SIGNAL)/ADC_MAX;
    float brake = (float)analogRead(BSE_SIGNAL)/ADC_MAX;


    if(throttle1 < APPS_DEADZONE) {
        globalStatus.ECUState = DRIVE_STANDBY;
        return;
    }
    if (fabs(throttle1 - throttle2) > 0.1) {
        globalStatus.ECUState = DRIVE_STANDBY;
        return;
    }
    if (brake >= BSE_DEADZONE && throttle1 >= 0.25) {
        globalStatus.ECUState = DRIVE_STANDBY;
        return;
    }

    
}

void drive_active_regen(void)
{
    // LOTS OF https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1214
    // Some math in https://github.com/Gaucho-Racing/VDM-24/blob/9ee4839ee6e5ce32a51602fe23723db5d23b1eaf/src/main.cpp#L1253

    
    if (true /*Throttle engaged*/)
        globalStatus.ECUState = DRIVE_ACTIVE_POWER;
    if (false /*Settings say no regen braking*/)
        globalStatus.ECUState = DRIVE_ACTIVE_IDLE;
    if (false /*TS ACTIVE button disabled*/ || false /*ACU shutdown*/ || false /*Critical error*/)
       globalStatus.ECUState = TS_DISCHARGE_OFF;
}
