#include <stdint.h>
#include <stdbool.h>

#ifndef INVERTER_H
    #define INVERTER_H
    typedef struct {
        uint16_t Set_AC_Current;
        uint16_t Set_DC_Current;
        uint16_t RPM_Limit;
        uint8_t Drive_Enable;   // Must be 1 to enable inverter
    } InverterSettings;

    extern volatile InverterSettings globalInverterSettings[4];
    void controlInverters(uint8_t driveEnable);
    void sendInverterCommand(void);

    extern volatile DTI_Data inverterData;

    long getERPM(void);
    float getDuty(void);
    int getVoltIn(void);
    float getACCurrent(void);
    float getDCCurrent(void);
    float getInvTemp(void);
    float getMotorTemp(void);
    uint8_t getFaults(void);
    float getCurrentD(void);
    float getCurrentQ(void);
    uint8_t getThrottleIn(void);
    uint8_t getBrakeIn(void);
    bool getD1(void);
    bool getD2(void);
    bool getD3(void);
    bool getD4(void);
    bool getDO1(void);
    bool getDO2(void);
    bool getDO3(void);
    bool getDO4(void);
    bool getDriveEnable(void);
    bool getCapTempLim(void);
    bool getDCCurrentLim(void);
    bool getDriveEnableLim(void);
    bool getIgbtAccelTempLim(void);
    bool getIgbtTempLim(void);
    bool getVoltInLim(void);
    bool getMotorAccelTempLim(void);
    bool getMotorTempLim(void);
    bool getRPMMinLimit(void);
    bool getRPMMaxLimit(void);
    bool getPowerLimit(void);
#endif
