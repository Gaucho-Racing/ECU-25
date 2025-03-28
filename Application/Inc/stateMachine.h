#include <stdint.h>

#ifndef STATEMACHINE_H
    #define STATEMACHINE_H

    #define HOW_LONG_TSDISCHARGE_UNTIL_ERROR_MS 5000
    #define TS_VOLTAGE_OFF_LIMIT 60

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

    /**
    General Low Voltage - On

    Once the GLVMS is set to ON this state is reached de facto.
    This is the first state reached on the board recieving power.
    The reciprocal off state is when the board has no power.
    Should a terrible error occur we may be thrown back here.

    However, we may configure boot to jump to `REFLASH_TUNE` instead.
    That is pending development. The other thought is to configure via CAN.

    Pretty much holds state until a CAN message comes in.
    */
    void glv_on(void);

    /**
    Precharge Engaged

    Once in `GLV_ON` and the TSMS is set to ON.

    Pretty much holds state.
    */
    void precharge_engaged(void);

    /**
    Precharging

    Once in `PRECHARGE_ENGAGED` and the ACU gives a precharge confirmation.

    Pretty much holds state.
    */
    void precharging(void);

    /**
    Precharge Complete

    Once in `PRECHARGING` and the ACU gives a precharge success confirmation.

    Pretty much holds state.
    */
    void precharge_complete(void);

    /**
    TD Discharge Off

    Complicated. Refer to the actual code for the best understanding.

    Once TSV is higher than 60 volts and we leave drive state for any reason.
    Discharges the TS so that HV systems can be safe iff there are no errors.
    Can be errorful or standardly running when this state is reached.

    Tries to reduce the HV TS if it is possible.
    */
    void ts_discharge_off(void);

    /**
    Reflash Tune

    Might read from the SD card on startup and configure some settings.
    Could backup parameters, change parameters, validate parameters, etc.

    Pending implementation. Passes to `GLV_ON` on success.
    */
    void reflash_tune(void);

    /**
    Error

    Complicated. Refer to the actual code for the best understanding.

    Attempts to send to discharge if TSV > 60 volts.

    Pretty much holds state until errors resolved.
    */
    void error(void);

    /**
    State Machine Tick

    Call as often as possible, handles all logic and internal systems.
    Effectively half of the ECU, works with the interrupt-focused `CANDler.c`.

    Ticks the state machine and calls the appropriate function.
    */
    void stateMachineTick(void);

#endif // STATEMACHINE_H
