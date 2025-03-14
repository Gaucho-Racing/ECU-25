#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <stdint.h>

typedef enum {
    GLV_ON = 1,
    PRECHARGE_ENGAGED = 2,
    PRECHARGING = 3,
    PRECHARGE_COMPLETE = 4,
    DRIVE_STANDBY = 5,
    DRIVE_ACTIVE_IDLE = 6,
    DRIVE_ACTIVE_POWER = 7,
    DRIVE_ACTIVE_REGEN = 8,
    TS_DISCHARGE_OFF = 9,
    REFLASH_TUNE = 10,
    ERRORSTATE = 11
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
        uint8_t ECUStatusMsgOne[8];
        uint8_t ECUStatusMsgTwo[8];
        uint8_t ECUStatusMsgThree[4];
    };
} StatusLump;

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
