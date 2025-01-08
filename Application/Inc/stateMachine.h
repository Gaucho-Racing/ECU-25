#include <stdint.h>

typedef enum {
    GLV_OFF,
    GLV_ON,
    PRECHARGE_ENGAGED,
    PRECHARGING,
    PRECHARGE_COMPLETE,
    DRIVE_STANDBY,
    DRIVE_ACTIVE_IDLE,
    DRIVE_ACTIVE_POWER,
    DRIVE_ACTIVE_REGEN,
    TS_DISCHARGE_OFF,
    REFLASH_TUNE,
    ERROR
} State;

typedef struct {
    int replaceMeWithUsefulThings;
} VehicleParameters;

typedef struct {
    int replaceMeWithUsefulThings;
} InformationToPassToState;

/*
Starting point, general low voltage off
*/
State* glv_off();

/*
General low voltage on
*/
State* glv_on();

/*
Precharge engaged
*/
State* precharge_engaged();

/*
Precharging
*/
State* precharging();

/*
Precharge complete
*/
State* precharge_complete();

/*
Ready to go, drive standby
*/
State* drive_standby();

/*
Drive active sub state, idle
*/
State* drive_active_idle();

/*
Drive active sub state, power
*/
State* drive_active_power();

/*
Drive active sub state, regen
*/
State* drive_active_regen();

/*
Shutting down, ts discharge off
*/
State* ts_discharge_off();

/*
Set new stuff, reflash tune
*/
State* reflash_tune();

/*
Error state, error
*/
State* error();

void stateMachineTick(State* state, InformationToPassToState info);
