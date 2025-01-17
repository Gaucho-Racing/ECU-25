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

/*
General low voltage on

When the grounded low voltage system is turned on, the microcontroller has power, 
but the motor controller is not enabled. This is the second state that the car will enter
after the ECU Flash is complete. Here it waits for the TS ACTIVE button to be pressed.
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

/*
CALL ME! Pass in the state and the info and it will automatically tick
*/
void stateMachineTick(State* state);