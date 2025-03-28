#include <stdint.h>
#include <stdbool.h>

#include "CANdler.h"

#ifndef INVERTER_H
    #define INVERTER_H

    typedef struct {
        uint16_t Set_AC_Current;
        uint16_t Set_DC_Current;
        uint16_t RPM_Limit;
        uint8_t Drive_Enable;
    } InverterSettings;

    extern volatile InverterSettings globalInverterSettings[3]; // Set to 4 iff fourth motor/inverter
    extern volatile DTI_Data inverterData;

    void controlInverters(bool driveEnable);
    void sendInverterCommand(void);

    bool inverter_getD1(void);
    bool inverter_getD2(void);
    bool inverter_getD3(void);
    bool inverter_getD4(void);
    bool inverter_getDO1(void);
    bool inverter_getDO2(void);
    bool inverter_getDO3(void);
    bool inverter_getDO4(void);
    bool inverter_getCapTempLim(void);
    bool inverter_getDCCurrentLim(void);
    bool inverter_getDriveEnableLim(void);
    bool inverter_getIgbtAccelTempLim(void);
    bool inverter_getIgbtTempLim(void);
    bool inverter_getVoltInLim(void);
    bool inverter_getMotorAccelTempLim(void);
    bool inverter_getMotorTempLim(void);
    bool inverter_getRPMMinLimit(void);
    bool inverter_getRPMMaxLimit(void);
    bool inverter_getPowerLimit(void);

#endif // INVERTER_H
