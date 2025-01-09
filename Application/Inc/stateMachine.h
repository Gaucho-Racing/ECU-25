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
void glv_off(State* state);

/*
General low voltage on
*/
void glv_on(State* state);

/*
Precharge engaged
*/
void precharge_engaged(State* state);

/*
Precharging
*/
void precharging(State* state);

/*
Precharge complete
*/
void precharge_complete(State* state);

/*
Ready to go, drive standby
*/
void drive_standby(State* state);

/*
Drive active sub state, idle
*/
void drive_active_idle(State* state);

/*
Drive active sub state, power
*/
void drive_active_power(State* state);

/*
Drive active sub state, regen
*/
void drive_active_regen(State* state);

/*
Shutting down, ts discharge off
*/
void ts_discharge_off(State* state);

/*
Set new stuff, reflash tune
*/
void reflash_tune(State* state);

/*
Error state, error
*/
void error(State* state);

void stateMachineTick(State* state, InformationToPassToState info);
