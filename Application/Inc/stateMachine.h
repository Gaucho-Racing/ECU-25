#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <stdint.h>

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
    uint8_t ECUState;
    uint8_t StatusBits[3];
    uint8_t PowerLevelTorqueMap;
    uint8_t MaxCellTemp;
    uint8_t AccumulatorStateOfCharge;
    uint8_t GLVStateOfCharge;
    uint16_t TractiveSystemVoltage;
    uint16_t VehicleSpeed;
    uint16_t FRWheelRPM;
    uint16_t FLWheelRPM;
    uint16_t RRWheelRPM;
    uint16_t RLWheelRPM;
    struct {
        int16_t ACCurrent;
        uint16_t Temp;
        uint16_t RPM;
    } inverters[4];
} StatusLump;

union StatusGotLumpy {
    StatusLump stateAll;
    uint64_t first;
    uint64_t second;
    uint32_t thrid;
};

typedef struct {
    uint8_t encoderBits;
    uint8_t buttonFlags;
} SteerSettings;

extern SteerSettings steerSettings;

extern StatusLump globalStatus;
extern uint8_t numberOfBadMessages;

/*
General low voltage on

When the grounded low voltage system is turned on, the microcontroller has power, 
but the motor controller is not enabled. This is the second state that the car will enter
after the ECU Flash is complete. Here it waits for the TS ACTIVE button to be pressed.
*/
void glv_on(StatusLump *status);

/*
Precharge engaged
*/
void precharge_engaged(StatusLump *status);

/*
Precharging
*/
void precharging(StatusLump *status);

/*
Precharge complete
*/
void precharge_complete(StatusLump *status);

/*
Drive standby
*/
void drive_standby(StatusLump *status);

/*
Shutting down, ts discharge off
*/
void ts_discharge_off(StatusLump *status);

/*
Set new stuff, reflash tune
*/
void reflash_tune(StatusLump *status);

/*
Error state, error
*/
void error(StatusLump *status);

/*
CALL ME! Pass in the state and the info and it will automatically tick
*/
void stateMachineTick(void);

#endif // STATEMACHIE_H
