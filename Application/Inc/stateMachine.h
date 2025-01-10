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
    ERROR
} State;

typedef struct {
    int replaceMeWithUsefulThings;
} VehicleParameters;

typedef struct {
    VehicleParameters vehicleParameters;
    unsigned long long millisecondsSinceLastMessage;
    unsigned long long communicationFrequency;
    int replaceMeWithUsefulThings;
} InformationToPassToState;

/*
General low voltage on
*/
void glv_on(State* state, InformationToPassToState info);

/*
Precharge engaged
*/
void precharge_engaged(State* state, InformationToPassToState info);

/*
Precharging
*/
void precharging(State* state, InformationToPassToState info);

/*
Precharge complete
*/
void precharge_complete(State* state, InformationToPassToState info);

/*
Ready to go, drive standby
*/
void drive_standby(State* state, InformationToPassToState info);

/*
Drive active sub state, idle
*/
void drive_active_idle(State* state, InformationToPassToState info);

/*
Drive active sub state, power
*/
void drive_active_power(State* state, InformationToPassToState info);

/*
Drive active sub state, regen
*/
void drive_active_regen(State* state, InformationToPassToState info);

/*
Shutting down, ts discharge off
*/
void ts_discharge_off(State* state, InformationToPassToState info);

/*
Set new stuff, reflash tune
*/
void reflash_tune(State* state, InformationToPassToState info);

/*
Error state, error
*/
void error(State* state, InformationToPassToState info);

void stateMachineTick(State* state, InformationToPassToState info);

/*
Will return true only when enough time has passed
ie: if (rateLimit(info)) { ... }
*/
_Bool rateLimit(InformationToPassToState info);