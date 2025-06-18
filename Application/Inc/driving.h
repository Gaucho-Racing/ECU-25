#include <stdbool.h>
#include <stdint.h>

#include "stateMachine.h"

#ifndef DRIVING_H
    #define DRIVING_H

    #define BRAKE_WINDOW_SIZE 100
    extern uint16_t brake_pressure_front_buffer[BRAKE_WINDOW_SIZE];
    extern uint16_t brake_pressure_rear_buffer[BRAKE_WINDOW_SIZE];
    extern uint16_t brake_pressure_front_sum;
    extern uint16_t brake_pressure_rear_sum;
    extern uint16_t brake_pressure_magic_index;
    extern uint16_t brake_pressure_front;
    extern uint16_t brake_pressure_rear;

    #define DRIVE_ACTIVE_POWER_REGEN_INTERVAL_MS 10

    #define APPS_DEADZONE 0.05f
    #define BSE_DEADZONE 0.05f
    #define REGEN_MPH 5

    // FIXME
    #define THROTTLE_MIN_1 680
    #define THROTTLE_MAX_1 820

    // FIXME
    #define THROTTLE_MIN_2 1560
    #define THROTTLE_MAX_2 1940

    // NOT USED
    #define AUX_MIN 0
    #define AUX_MAX 4095
    // NOT USED

    // FIXME
    #define BRAKE_F_MIN 772
    #define BRAKE_F_MAX 925

    // FIXME
    #define BRAKE_R_MIN 538
    #define BRAKE_R_MAX 650
   
    #define MAX_CURRENT_REAR 240.0f // A
    #define MAX_CURRENT_FORWARD 105.0f // A
    #define NOMINAL_CURRENT_FORWARD 41.0f // A
    #define MAX_REVERSE_CURRENT 40.0f // A


    #define MAX_AMK_HEAT_CAP 11587
    #define MIN_BAT_TEMP_REGEN_THROTTLE 50 // C

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
