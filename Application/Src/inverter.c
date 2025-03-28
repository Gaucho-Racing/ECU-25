#include <stdint.h>
#include <stdbool.h>

#include "utils.h"
#include "inverter.h"
#include "fdcan.h"
#include "msgIDs.h"

volatile DTI_Data inverterData = {0};

volatile InverterSettings globalInverterSettings[3] = { // Set to 4 iff fourth motor/inverter
    {0, .RPM_Limit = 1, 0},
    {0, .RPM_Limit = 1, 0},
    {0, .RPM_Limit = 1, 0},
//  {0, .RPM_Limit = 1, 0}, // Enable iff fourth motor/inverter
};

volatile int32_t lastInverterPingMillis = -1;

void sendInverterCommand(void)
{
    if (millis() - lastInverterPingMillis >= 50) // Must send every 100 ms
    {
        lastInverterPingMillis = millis();
        writeMessage(1, MSG_INVERTER_COMMAND, GR_GR_INVERTER_1, (uint8_t*)&globalInverterSettings[0], 7);
        writeMessage(1, MSG_INVERTER_COMMAND, GR_GR_INVERTER_2, (uint8_t*)&globalInverterSettings[1], 7);
        writeMessage(1, MSG_INVERTER_COMMAND, GR_GR_INVERTER_3, (uint8_t*)&globalInverterSettings[2], 7);
//      writeMessage(1, MSG_INVERTER_COMMAND, GR_GR_INVERTER_4, (uint8_t*)&globalInverterSettings[3], 7);   // Enable iff fourth motor/inverter
    }
}

void controlInverters(bool driveEnable)
{
    globalInverterSettings[0] = (InverterSettings){0, 0, 0, (uint8_t)driveEnable};
    globalInverterSettings[1] = (InverterSettings){0, 0, 0, (uint8_t)driveEnable};
    globalInverterSettings[2] = (InverterSettings){0, 0, 0, (uint8_t)driveEnable};
//  globalInverterSettings[3] = (InverterSettings){0, 0, 0, (uint8_t)driveEnable};  // Enable iff fourth motor/inverter

    sendInverterCommand();
}

/* See https://github.com/Gaucho-Racing/VDM-24/blob/main/src/Nodes.h for all of below */

/**
DTI Data 5 - Digital Input 1

@returns Bool representing active
*/
bool inverter_getDigitalInput1(void)
{
    return ((inverterData.data[4][2] & 0x80) == 0x80);
}

/**
DTI Data 5 - Digital Input 2

@returns Bool representing active
*/
bool inverter_getDigitalInput2(void)
{
    return ((inverterData.data[4][2] & 0x40) == 0x40);
}

/**
DTI Data 5 - Digital Input 3

@returns Bool representing active
*/
bool inverter_getDigitalInput3(void)
{
    return ((inverterData.data[4][2] & 0x20) == 0x20);
}

/**
DTI Data 5 - Digital Input 4

@returns Bool representing active
*/
bool inverter_getDigitalInput4(void)
{
    return ((inverterData.data[4][2] & 0x10) == 0x10);
}

/**
DTI Data 5 - Digital Output 1

@returns Bool representing active
*/
bool inverter_getDigitalOutput1(void)
{
    return ((inverterData.data[4][2] & 0x08) == 0x08);
}

/**
DTI Data 5 - Digital Output 2

@returns Bool representing active
*/
bool inverter_getDigitalOutput2(void)
{
    return ((inverterData.data[4][2] & 0x04) == 0x04);
}

/**
DTI Data 5 - Digital Output 3

@returns Bool representing active
*/
bool inverter_getDigitalOutput3(void)
{
    return ((inverterData.data[4][2] & 0x02) == 0x02);
}

/**
DTI Data 5 - Digital Output 4

@returns Bool representing active
*/
bool inverter_getDigitalOutput4(void)
{
    return ((inverterData.data[4][2] & 0x01) == 0x01);
}

/**
DTI Data 5 - Capacitor Temperature Limit

@returns Bool representing active
*/
bool inverter_getCapacitorTempLimit(void)
{
    return ((inverterData.data[4][4] & 0x80) == 0x80);
}

/**
DTI Data 5 - DC Current Limit

@returns Bool representing active
*/
bool inverter_getDCCurrentLimit(void)
{
    return ((inverterData.data[4][4] & 0x40) == 0x40);
}

/**
DTI Data 5 - Drive Enable Limit

@returns Bool representing active
*/
bool inverter_getDriveEnableLimit(void)
{
    return ((inverterData.data[4][4] & 0x20) == 0x20);
}

/**
DTI Data 5 - IGBT Acceleration Temperature Limit

@returns Bool representing active
*/
bool inverter_getIgbtAccelerationTemperatureLimit(void)
{
    return ((inverterData.data[4][4] & 0x10) == 0x10);
}

/**
DTI Data 5 - IGBT Temperature Limit

@returns Bool representing active
*/
bool inverter_getIgbtTemperatureLimit(void)
{
    return ((inverterData.data[4][4] & 0x08) == 0x08);
}

/**
DTI Data 5 - Input Voltage Limit

@returns Bool representing active
*/
bool inverter_getVoltageInputLimit(void)
{
    return ((inverterData.data[4][4] & 0x04) == 0x04);
}

/**

*/
bool inverter_getMotorAccelerationTemperatureLimit(void)
{
    return ((inverterData.data[4][4] & 0x02) == 0x02);
}

bool inverter_getMotorTemperatureLimit(void)
{
    return ((inverterData.data[4][4] & 0x01) == 0x01);
}

bool inverter_getRPMMinLimit(void)
{
    return ((inverterData.data[4][5] & 0x80) == 0x80);
}

bool inverter_getRPMMaxLimit(void)
{
    return ((inverterData.data[4][5] & 0x40) == 0x40);
}

bool inverter_getPowerLimit(void)
{
    return ((inverterData.data[4][5] & 0x20) == 0x20);
}
