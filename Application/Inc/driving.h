#include <stdbool.h>

#include "stateMachine.h"

#ifndef DRIVING_H
    #define DRIVING_H

    #define DRIVE_ACTIVE_POWER_REGEN_INTERVAL_MS 10

    #define APPS_DEADZONE 0.05f
    #define BSE_DEADZONE 0.05f
    #define REGEN_MPH 5

    #define THROTTLE_MIN_1 631
    #define THROTTLE_MAX_1 887

    #define THROTTLE_MIN_2 1510
    #define THROTTLE_MAX_2 2017

    /*
    #define THROTTLE_MIN_1 0.315445f
    #define THROTTLE_MAX_1 0.443382562278f

    #define THROTTLE_MIN_2 0.7548975f
    #define THROTTLE_MAX_2 1.00845551601f
    */

    #define BRAKE_MIN 0 // FIXME change when value is known
    #define BRAKE_MAX 4096 // FIXME change when value is known
    #define MAX_CURRENT_REAR 240.0f // A
    #define MAX_CURRENT_FORWARD 105.0f // A
    #define NOMINAL_CURRENT_FORWARD 41.0f // A
    #define NOMINAL_REGEN_CURRENT 10.0f // FIXME change when value is known


    #define MAX_AMK_HEAT_CAP 11587
    #define MIN_BAT_TEMP_REGEN_THROTTLE 57 // C

    #define GEAR_RATIO (42.0f / 13.0f)    // 42T driven and 13T driving
    #define MOTOR_POLE_PAIRS 10.0f
    #define WHEEL_RADIUS_IN 8.0f
    #define MPH_TO_INCH_PER_MIN_CONV 1056.0f

    #define POWERLEVEL_TORQUEMAP_RESET 0xFF
    #define GLOBALSTATUS_WHEEL_RPM_ADJUSTMENT 3276

    extern volatile bool BSE_APPS_violation;
    extern volatile float minAmkHeatCapThrottlePercent;
    
    /**
    Vehicle Speed in MPH

    Valid once message `DTI Data 1` has been recieved.
    */
   float vehicleSpeedMPH(void);

    /**
    Drive Standby

    Once `PRECHARGE_COMPLETE` and brake depressed and RTD on or noncritical violation.

    Awaits a valid torque request or an error of some form.
    */
    void drive_standby(void);

    /**
    Drive Active - Idle

    Not requesting torque, but still driving.
    Times out if no action requested.

    Not implemented yet, pending TV.
    */
    void drive_active_idle(void);

    /**
    Drive Active - Power

    Requesting torque, still driving.

    Not implemented yet, pending TV.
    */
    void drive_active_power(void);

    /**
    Drive Active - Regenerative

    Requesting negative torque to slow the car and recharge the battery.

    Not implemented yet, pending TV.
    */
    void drive_active_regen(void);

#endif // DRIVING_H
