#include <stdint.h>
#include <stdbool.h>

#include "utils.h"
#include "inverter.h"

// I am so sorry...
// https://github.com/Gaucho-Racing/VDM-24/blob/main/src/Nodes.h

uint8_t data[5][8];

long getERPM(void) {return(((long)data[0][0] << 24) + ((long)data[0][1] << 16) + ((long)data[0][2] << 8) + data[0][3]);} //rpm/pole pairs
float getDuty(void) {return((((long)data[0][4] << 8) + data[0][5])/10);} //i think [0,100]. Related to top speed
int getVoltIn(void) {return(((long)data[0][6] << 8) + data[0][7]);}
float getACCurrent(void) {return((float)(((uint16_t)(data[1][0]) << 8) + data[1][1])/10.0);}
float getDCCurrent(void) {return(((long)((uint16_t)(data[1][2]) << 8) + data[1][3])/10.0);}
float getInvTemp(void) {return((((long)data[2][0] << 8) + data[2][1])/10.0);} //Deg C
float getMotorTemp(void) {return((((long)data[2][2] << 8) + data[2][3])/10.0);} //Deg C
uint8_t getFaults(void) {return data[2][4];}
float getCurrentD(void) {return((((long)data[3][0] << 24) + ((long)data[3][1] << 16) + ((long)data[3][2] << 8) + data[3][3])/100.0);}  //FOC current (don't need)
float getCurrentQ(void) {return((((long)data[3][4] << 24) + ((long)data[3][5] << 16) + ((long)data[3][6] << 8) + data[3][7])/100.0);}  //FOC current (don't need)
uint8_t getThrottleIn(void) {return data[4][0];}  //Received throttle signal by the invertor
uint8_t getBrakeIn(void) {return data[4][1];}  //Received brake signal by the invertor
bool getD1(void) {return ((data[4][2] & 0x80) == 0x80);}  //Digital input read
bool getD2(void) {return ((data[4][2] & 0x40) == 0x40);}  //Digital input read
bool getD3(void) {return ((data[4][2] & 0x20) == 0x20);}  //Digital input read
bool getD4(void) {return ((data[4][2] & 0x10) == 0x10);}  //Digital input read
bool getDO1(void) {return ((data[4][2] & 0x08) == 0x08);}  //Digital output write
bool getDO2(void) {return ((data[4][2] & 0x04) == 0x04);}  //Digital output write
bool getDO3(void) {return ((data[4][2] & 0x02) == 0x02);}  //Digital output write
bool getDO4(void) {return ((data[4][2] & 0x01) == 0x01);}  //Digital output write
bool getDriveEnable(void) {return ((data[4][3] & 0x01) == 0x01);} //These are setting that can be changed (prob don't need these)
bool getCapTempLim(void) {return ((data[4][4] & 0x80) == 0x80);}//         ^
bool getDCCurrentLim(void) {return ((data[4][4] & 0x40) == 0x40);}//       ^
bool getDriveEnableLim(void) {return ((data[4][4] & 0x20) == 0x20);}//     ^
bool getIgbtAccelTempLim(void) {return ((data[4][4] & 0x10) == 0x10);}//   ^
bool getIgbtTempLim(void) {return ((data[4][4] & 0x08) == 0x08);}//        ^
bool getVoltInLim(void) {return ((data[4][4] & 0x04) == 0x04);}//          ^
bool getMotorAccelTempLim(void) {return ((data[4][4] & 0x02) == 0x02);}//  ^
bool getMotorTempLim(void) {return ((data[4][4] & 0x01) == 0x01);}//       ^
bool getRPMMinLimit(void) {return ((data[4][5] & 0x80) == 0x80);}//        ^
bool getRPMMaxLimit(void) {return ((data[4][5] & 0x40) == 0x40);}//        ^
bool getPowerLimit(void) {return ((data[4][5] & 0x20) == 0x20);}//         ^
