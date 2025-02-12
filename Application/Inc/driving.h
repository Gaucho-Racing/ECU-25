#ifndef DRIVING_H
#define DRIVING_H

#include "stateMachine.h"

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