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

typedef union {
    struct {
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
    };

    struct {
        uint64_t first;
        uint64_t second;
        uint32_t third;
    };
} StatusLump;

typedef struct {
    uint8_t torqueMapCurrentEncoder;
    uint8_t regenEncoder;
    uint8_t buttonFlags;
} SteerSettings;

extern volatile SteerSettings *globalSteerSettings;

extern volatile StatusLump globalStatus;
extern volatile uint8_t numberOfBadMessages;

/*
General low voltage on

When the grounded low voltage system is turned on, the microcontroller has power, 
but the motor controller is not enabled. This is the second state that the car will enter
after the ECU Flash is complete. Here it waits for the TS ACTIVE button to be pressed.
*/
void glv_on(void);

/*
Precharge engaged
*/
void precharge_engaged(void);

/*
Precharging
*/
void precharging(void);

/*
Precharge complete
*/
void precharge_complete(void);

/*
Shutting down, ts discharge off
*/
void ts_discharge_off(void);

/*
Set new stuff, reflash tune
*/
void reflash_tune(void);

/*
Error state, error
*/
void error(void);

/*
CALL ME! Pass in the state and the info and it will automatically tick
*/
void stateMachineTick(void);

#endif // STATEMACHINE_H