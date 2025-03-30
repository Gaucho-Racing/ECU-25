#include <stdbool.h>

#include "stateMachine.h"

#ifndef DRIVING_H
    #define DRIVING_H

    #define APPS_DEADZONE 0.05
    #define BSE_DEADZONE 0.05
    #define REGEN_MPH 5

    #define GEAR_RATIO 3.55
    #define MOTOR_POLE_PAIRS 10.0
    #define WHEEL_RADIUS_IN 8.0 // inches

    #define POWERLEVEL_TORQUEMAP_RESET 0xFF
    #define GLOBALSTATUS_WHEEL_RPM_ADJUSTMENT 3276.8

    extern volatile bool BSE_APPS_violation;

    /**
    Converts from `globalStatus.??WheelRPM` to actual RPM of given wheel

    @param ECUStatusMsgWheelRPM A given wheel's ECU Status RPM: globalStatus.??WheelRPM
    @returns int16_t A signed integer representing the real RPM of the provided wheel

    Number is already scaled on reception, just needs to be shifted to be used
    */
   int16_t convertFromECUStatusRPMToRealRPM(uint16_t ECUStatusMsgWheelRPM);

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
