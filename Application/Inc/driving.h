#ifndef DRIVING_H
#define DRIVING_H

/*
Ready to go, drive standby
*/
void drive_standby(StatusLump *status);

/*
Drive active sub state, idle
*/
void drive_active_idle(StatusLump *status);

/*
Drive active sub state, power
*/
void drive_active_power(StatusLump *status);

/*
Drive active sub state, regen
*/
void drive_active_regen(StatusLump *status);

#endif // DRIVING_H