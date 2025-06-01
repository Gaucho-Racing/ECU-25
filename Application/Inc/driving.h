#include <stdbool.h>

#include "stateMachine.h"

#ifndef DRIVING_H
    #define DRIVING_H

    #define APPS_DEADZONE 0.05
    #define BSE_DEADZONE 0.05
    #define REGEN_MPH 5

    #define THROTTLE_MIN_1 0.315445
    #define THROTTLE_MAX_1 0.443382562278


    #define THROTTLE_MIN_2 0.7548975
    #define THROTTLE_MAX_2 1.00845551601

    #define BRAKE_MIN 0.0 // FIXME change when value is known
    #define BRAKE_MAX 5.0 // FIXME change when value is known
    #define MAX_CURRENT_REAR 240.0 // A
    #define MAX_CURRENT_FORWARD 105.0 // A
    #define NOMINAL_CURRENT_FORWARD 41.0 // A

    #define MAX_AMK_HEAT_CAP 11587

    #define GEAR_RATIO 3.55
    #define MOTOR_POLE_PAIRS 10.0
    #define WHEEL_RADIUS_IN 8.0 // inches
    #define MPH_TO_INCH_PER_MIN_CONV 1056.0

    #define POWERLEVEL_TORQUEMAP_RESET 0xFF
    #define GLOBALSTATUS_WHEEL_RPM_ADJUSTMENT 3276

    extern volatile bool BSE_APPS_violation;
    extern volatile uint16_t heatCapacity;
    extern volatile int32_t lastHeatCapacityUpdateMillis;
    
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
