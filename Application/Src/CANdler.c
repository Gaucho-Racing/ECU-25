#include <stdbool.h>

#include "CANdler.h"
#include "stateMachine.h"
#include "msgIDs.h"
#include "pinging.h"
#include "adc.h"
#include "utils.h"

uint8_t errorFlagBitsCan = 0;


uint16_t findTernaryMax(const uint16_t a, const uint16_t b, const uint16_t c)
{
    if (a > b && a > c)
        return a;
    else if (b > a && b > c)
        return b;
    else
        return c;
}

void handleCANMessage(uint16_t msgID, uint8_t srcID, uint8_t *data, uint32_t length, uint32_t timestamp) {
    switch(msgID) {
        case MSG_DEBUG:
            if (length > 64) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            char* string = (char*)data;

            break;
        case MSG_PING:
            if (length != 4) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            respondToPing(srcID, *(uint32_t*)data);

            break;
        case MSG_ACU_STATUS_1:
            if (length != 8) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            ACU_Status_MsgOne* acuMsgOne = (ACU_Status_MsgOne*)data;
            globalStatus.AccumulatorStateOfCharge = acuMsgOne->Accumulator_SOC;
            globalStatus.GLVStateOfCharge = acuMsgOne->GLV_SOC;
            globalStatus.TractiveSystemVoltage = acuMsgOne->TS_Voltage;

            break;
        case MSG_ACU_STATUS_2:
            if (length != 8) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            ACU_Status_MsgTwo* acuMsgTwo = (ACU_Status_MsgTwo*)data;
            globalStatus.MaxCellTemp = acuMsgTwo->Max_Cell_Temp;

            //errorFlagBitsCan logic
            if(acuMsgTwo->Error_Warning_Bits != 0x00 && (errorFlagBitsCan == 0 || errorFlagBitsCan == 2)){
                errorFlagBitsCan += 1;
            }
            else if(acuMsgTwo->Error_Warning_Bits == 0x00 && (errorFlagBitsCan == 1 || errorFlagBitsCan == 3)){
                errorFlagBitsCan -= 1;
            }

            //Error handling and leaving error state
            if ((errorFlagBitsCan || acuMsgTwo->Precharge_Error == 0x01) && globalStatus.TractiveSystemVoltage >= 60)
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }
            else if(errorFlagBitsCan || acuMsgTwo->Precharge_Error == 0x01){
                globalStatus.ECUState = ERRORSTATE;
            }
            else if (globalStatus.ECUState == ERRORSTATE || (globalStatus.ECUState == TS_DISCHARGE_OFF && globalStatus.TractiveSystemVoltage < 60))
            {
                globalStatus.ECUState = GLV_ON;
            }
            
            // IR- -> 1 = ACU Precharge Confirmation
            if((globalStatus.ECUState == PRECHARGE_ENGAGED) && (getBit(acuMsgTwo->IR_State_Software_Latch_Bits, 0) == 0x01))
            {
                globalStatus.ECUState = PRECHARGING;
            }

            // If IR- ever becomes 0 while not in GLV_ON or PRECHARGE_ENGAGED, that is a precharge cancellation and it must start discharging.
            if((getBit(acuMsgTwo->IR_State_Software_Latch_Bits, 0)) == 0x00 && (globalStatus.ECUState != GLV_ON) && (globalStatus.ECUState != PRECHARGE_ENGAGED))
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }

            // If it is precharging with IR- closed and then IR+ goes closed as well, precharge is complete (success confirmation)
            // IR+ -> 1 is precharge success confirmation
            if((getBit(acuMsgTwo->IR_State_Software_Latch_Bits, 0) == 0x01) && (getBit(acuMsgTwo->IR_State_Software_Latch_Bits, 1) == 0x01) && (globalStatus.ECUState == PRECHARGING))
            {
                globalStatus.ECUState = PRECHARGE_COMPLETE;
            }

            //If IR+ ever opens on or after the precharging complete state, start discharging
            if(getBit(acuMsgTwo->IR_State_Software_Latch_Bits, 1) == 0x0 && globalStatus.ECUState != GLV_ON && globalStatus.ECUState != PRECHARGE_ENGAGED && globalStatus.ECUState != PRECHARGING)
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }

            //If ACU software latch ever opens IR- ever opens while IR+ is closed, something has gone wrong
            if(getBit(acuMsgTwo->IR_State_Software_Latch_Bits, 2) == 0x0 || (getBit(acuMsgTwo->IR_State_Software_Latch_Bits, 1) == 0x1 && getBit(acuMsgTwo->IR_State_Software_Latch_Bits, 0) == 0x0))
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }

            break;
        case MSG_ACU_STATUS_3:
            if (length != 8) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            ACU_Status_MsgThree* acuMsgThree = (ACU_Status_MsgThree*)data;

            break;

        case MSG_DC_DC_STATUS:
            if (length != 8) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Msg_Lv_Dc_Dc_Status* msgLv = (Msg_Lv_Dc_Dc_Status*)data;
            
            break;
        case MSG_INVERTER_STATUS_1:
            if (length != 6) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Inverter_Status_Msg_One* msgGriOne = (Inverter_Status_Msg_One*)data;

            break;
        case MSG_INVERTER_STATUS_2:
            if (length != 6) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Inverter_Status_Msg_Two* msgGriTwo = (Inverter_Status_Msg_Two*)data;

            break;
        case MSG_INVERTER_STATUS_3:
            if (length != 3) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Inverter_Status_Msg_Three* msgGriThree = (Inverter_Status_Msg_Three*)data;

            globalStatus.VehicleSpeed = (globalStatus.RRWheelRPM + globalStatus.RLWheelRPM) * 3.141592653589 * 16 / 2 / 3.55 / 1056.0;  // FIXME: Fix the math, copied from VDM-24...

            if(msgGriThree->fault_map == 0x00 && (errorFlagBitsCan == 2 || errorFlagBitsCan == 3)){
                errorFlagBitsCan -= 2;
            }

            else if(msgGriThree->fault_map != 0x00 && (errorFlagBitsCan == 0 || errorFlagBitsCan == 1)){
                errorFlagBitsCan += 2;
            }

            if (errorFlagBitsCan && globalStatus.TractiveSystemVoltage >= 60)
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }
            else if(errorFlagBitsCan){
                globalStatus.ECUState = ERRORSTATE;
            }
            else if(globalStatus.ECUState == ERRORSTATE || (globalStatus.ECUState == TS_DISCHARGE_OFF && globalStatus.TractiveSystemVoltage < 60))
            {
                globalStatus.ECUState = GLV_ON;
            }

            break;
        case MSG_DASH_STATUS:
            if (length != 1) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }
            // Isn't rtd a port? Why is rtd_on and rtd_off needed here?
            bool ts_active = getBit(*data, 0);
            bool ts_off = getBit(*data, 1);
            bool rtd_on = getBit(*data, 2);
            bool rtd_off = getBit(*data, 3);
            bool ams_led = getBit(*data, 4);
            bool imd_led = getBit(*data, 5);

            if(ts_active && globalStatus.ECUState == GLV_ON){
                globalStatus.ECUState = PRECHARGE_ENGAGED;
            }
            
            else if (ts_off && globalStatus.ECUState == PRECHARGE_ENGAGED){
                globalStatus.ECUState = GLV_ON;
            }
            // If it is not in GLV_ON, PRECHARGE_ENGAGED or ERRORSTATE, if ts_off is ever true it must go to discharge
            else if (ts_off && globalStatus.ECUState != ERRORSTATE){
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }
            
            break;

        // not done - refer to lines 774,884, 904 from VDM-24 (main.cpp)
        case MSG_DASH_CONFIG:
            if (length != 7) {
                numberOfBadMessages++;
                return;
            }
            else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }
            
            
            break;

        case MSG_FAN_STATUS:
            if (length != 5) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Fan_Status_Msg* msgFan = (Fan_Status_Msg*)data;

            break;

        case MSG_STEERING_STATUS:
            if (length != 4) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }
            
            steerSettings.currentEncoder = data[0];
            steerSettings.torqueMapEncoder = data[1];
            steerSettings.regenEncoder = data[2];
            steerSettings.buttonFlags ^= data[3];
    }
}
