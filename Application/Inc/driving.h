#ifndef DRIVING_H
#define DRIVING_H

#include "stateMachine.h"

#define APPS_DEADZONE 0.05
#define BSE_DEADZONE 0.05
#define REGEN_MPH 5

#define GEAR_RATIO 3.55
#define MOTOR_POLE_PAIRS 10.0
#define WHEEL_RADIUS_IN 8.0 // inches

/*
Ready to go, drive standby
*/
void drive_standby(void);

/*
Drive active sub state, idle
*/
void drive_active_idle(void);

/*
Drive active sub state, power
*/
void drive_active_power(void);

/*
Drive active sub state, regen
*/
void drive_active_regen(void);

#endif // DRIVING_H
