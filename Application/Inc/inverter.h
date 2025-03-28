#include <stdint.h>
#include <stdbool.h>

#include "CANdler.h"

#ifndef INVERTER_H
    #define INVERTER_H

    #define INVERTER_MINIMUM_SENDING_TIME_MS 50

    typedef struct {
        uint16_t Set_AC_Current;
        uint16_t Set_DC_Current;
        uint16_t RPM_Limit;
        uint8_t Drive_Enable;
    } InverterSettings;

    extern volatile InverterSettings globalInverterSettings[3]; // Set to 4 iff fourth motor/inverter
    extern volatile DTI_Data globalInverterData;

    void controlInverters(bool driveEnable);
    void sendInverterCommand(void);

#endif // INVERTER_H
