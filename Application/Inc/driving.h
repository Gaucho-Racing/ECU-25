#ifndef DRIVING_H
#define DRIVING_H

#include "stateMachine.h"

#define APPS_DEADZONE 0.05
#define BSE_DEADZONE 0.05
#define REGEN_MPH 5

#define GEAR_RATIO 3.55
#define MOTOR_POLE_PAIRS 10.0
#define WHEEL_RADIUS_IN 8.0 // inches

typedef struct {
    uint16_t Set_AC_Current;
    uint16_t Set_DC_Current;
    uint16_t RPM_Limit;
    uint8_t Drive_Enable;   // Must be 1 to enable inverter
} InverterSettings;

typedef union {
    struct {
        InverterSettings inverter[4];
    };

    struct {
        uint8_t firstMsg;
        uint8_t secondMsg;
        uint8_t thirdMsg;
        uint8_t fourthMsg;
    };
} InverterLump;

extern volatile InverterLump globalInverterSettings;
extern volatile bool BSE_APPS_violation;

/*
Send the inverter command message to control the inverters based off of the globalInverterSettings
*/
void sendInverterCommand(void);

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
