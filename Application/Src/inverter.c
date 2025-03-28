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
    if(millis() - lastInverterPingMillis >= 50) // Must send every 100 ms
    {
        lastInverterPingMillis = millis();
        writeMessage(1, MSG_INVERTER_COMMAND, GR_GR_INVERTER_1, (uint8_t*)&globalInverterSettings[0], 7);
        writeMessage(1, MSG_INVERTER_COMMAND, GR_GR_INVERTER_2, (uint8_t*)&globalInverterSettings[1], 7);
        writeMessage(1, MSG_INVERTER_COMMAND, GR_GR_INVERTER_3, (uint8_t*)&globalInverterSettings[2], 7);
//      writeMessage(1, MSG_INVERTER_COMMAND, GR_GR_INVERTER_4, (uint8_t*)&globalInverterSettings[3], 7);   // Enable iff fourth motor/inverter
    }
}

void controlInverters(uint8_t driveEnable)
{
    globalInverterSettings[0] = (InverterSettings){0, 0, 0, driveEnable};
    globalInverterSettings[1] = (InverterSettings){0, 0, 0, driveEnable};
    globalInverterSettings[2] = (InverterSettings){0, 0, 0, driveEnable};
//  globalInverterSettings[3] = (InverterSettings){0, 0, 0, driveEnable};  // Enable iff fourth motor/inverter

    sendInverterCommand();
}

// See https://github.com/Gaucho-Racing/VDM-24/blob/main/src/Nodes.h for below
// There is exactly 1 DTI per https://discord.com/channels/756738476887638107/1256764134872060007/1355067348423540757

uint32_t getERPM(void) {return(((uint32_t)inverterData.data[0][0] << 24) + ((uint32_t)inverterData.data[0][1] << 16) + ((uint32_t)inverterData.data[0][2] << 8) + inverterData.data[0][3]);} //rpm/pole pairs
float getDuty(void) {return((((uint32_t)inverterData.data[0][4] << 8) + inverterData.data[0][5])/10);} //i think [0,100]. Related to top speed
uint32_t getVoltIn(void) {return(((uint32_t)inverterData.data[0][6] << 8) + inverterData.data[0][7]);}
float getACCurrent(void) {return((float)(((uint16_t)(inverterData.data[1][0]) << 8) + inverterData.data[1][1])/10.0);}
float getDCCurrent(void) {return(((uint32_t)((uint16_t)(inverterData.data[1][2]) << 8) + inverterData.data[1][3])/10.0);}
float getInvTemp(void) {return((((uint32_t)inverterData.data[2][0] << 8) + inverterData.data[2][1])/10.0);} //Deg C
float getMotorTemp(void) {return((((uint32_t)inverterData.data[2][2] << 8) + inverterData.data[2][3])/10.0);} //Deg C
uint8_t getFaults(void) {return inverterData.data[2][4];}
float getCurrentD(void) {return((((uint32_t)inverterData.data[3][0] << 24) + ((uint32_t)inverterData.data[3][1] << 16) + ((uint32_t)inverterData.data[3][2] << 8) + inverterData.data[3][3])/100.0);}  //FOC current (don't need)
float getCurrentQ(void) {return((((uint32_t)inverterData.data[3][4] << 24) + ((uint32_t)inverterData.data[3][5] << 16) + ((uint32_t)inverterData.data[3][6] << 8) + inverterData.data[3][7])/100.0);}  //FOC current (don't need)
uint8_t getThrottleIn(void) {return inverterData.data[4][0];}  //Received throttle signal by the invertor
uint8_t getBrakeIn(void) {return inverterData.data[4][1];}  //Received brake signal by the invertor
bool getD1(void) {return ((inverterData.data[4][2] & 0x80) == 0x80);}  //Digital input read
bool getD2(void) {return ((inverterData.data[4][2] & 0x40) == 0x40);}  //Digital input read
bool getD3(void) {return ((inverterData.data[4][2] & 0x20) == 0x20);}  //Digital input read
bool getD4(void) {return ((inverterData.data[4][2] & 0x10) == 0x10);}  //Digital input read
bool getDO1(void) {return ((inverterData.data[4][2] & 0x08) == 0x08);}  //Digital output write
bool getDO2(void) {return ((inverterData.data[4][2] & 0x04) == 0x04);}  //Digital output write
bool getDO3(void) {return ((inverterData.data[4][2] & 0x02) == 0x02);}  //Digital output write
bool getDO4(void) {return ((inverterData.data[4][2] & 0x01) == 0x01);}  //Digital output write
bool getDriveEnable(void) {return ((inverterData.data[4][3] & 0x01) == 0x01);} //These are setting that can be changed (prob don't need these)
bool getCapTempLim(void) {return ((inverterData.data[4][4] & 0x80) == 0x80);}//         ^
bool getDCCurrentLim(void) {return ((inverterData.data[4][4] & 0x40) == 0x40);}//       ^
bool getDriveEnableLim(void) {return ((inverterData.data[4][4] & 0x20) == 0x20);}//     ^
bool getIgbtAccelTempLim(void) {return ((inverterData.data[4][4] & 0x10) == 0x10);}//   ^
bool getIgbtTempLim(void) {return ((inverterData.data[4][4] & 0x08) == 0x08);}//        ^
bool getVoltInLim(void) {return ((inverterData.data[4][4] & 0x04) == 0x04);}//          ^
bool getMotorAccelTempLim(void) {return ((inverterData.data[4][4] & 0x02) == 0x02);}//  ^
bool getMotorTempLim(void) {return ((inverterData.data[4][4] & 0x01) == 0x01);}//       ^
bool getRPMMinLimit(void) {return ((inverterData.data[4][5] & 0x80) == 0x80);}//        ^
bool getRPMMaxLimit(void) {return ((inverterData.data[4][5] & 0x40) == 0x40);}//        ^
bool getPowerLimit(void) {return ((inverterData.data[4][5] & 0x20) == 0x20);}//         ^
// FIXME Why are so many of these bools? (Copied from VDM-24 as such)
