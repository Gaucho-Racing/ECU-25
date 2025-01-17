typedef enum {
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
    ERRORSTATE
} State;

typedef struct {
    int replaceMeWithUsefulThings;
} VehicleParameters;

/*
General low voltage on

When the grounded low voltage system is turned on, the microcontroller has power, 
but the motor controller is not enabled. This is the second state that the car will enter
after the ECU Flash is complete. Here it waits for the TS ACTIVE button to be pressed.
*/
void glv_on(State* state, InformationToPassToState* info);

/*
Precharge engaged
*/
void precharge_engaged(State* state, InformationToPassToState* info);

/*
Precharging
*/
void precharging(State* state, InformationToPassToState* info);

/*
Precharge complete
*/
void precharge_complete(State* state, InformationToPassToState* info);

/*
Ready to go, drive standby
*/
void drive_standby(State* state, InformationToPassToState* info);

/*
Drive active sub state, idle
*/
void drive_active_idle(State* state, InformationToPassToState* info);

/*
Drive active sub state, power
*/
void drive_active_power(State* state, InformationToPassToState* info);

/*
Drive active sub state, regen
*/
void drive_active_regen(State* state, InformationToPassToState* info);

/*
Shutting down, ts discharge off
*/
void ts_discharge_off(State* state, InformationToPassToState* info);

/*
Set new stuff, reflash tune
*/
void reflash_tune(State* state, InformationToPassToState* info);

/*
Error state, error
*/
void error(State* state, InformationToPassToState* info);

/*
CALL ME! Pass in the state and the info and it will automatically tick
*/
void stateMachineTick(State* state);