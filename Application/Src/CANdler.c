#include <stdbool.h>
#include <stdint.h>

#include "CANdler.h"
#include "stateMachine.h"
#include "msgIDs.h"
#include "pinging.h"
#include "adc.h"
#include "fdcan.h"
#include "grIDs.h"
#include "utils.h"

volatile uint8_t errorFlagBitsCan = 0;   // Only R/W inside of interrupts

volatile uint8_t globalSteeringStatusRegen = 0;
volatile uint8_t globalSteeringStatusButtonMap = 0;

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
    UNUSED(timestamp);

    switch(msgID) {
        case MSG_DEBUG_2_0:
            if (length > 8) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            char* debug2String = (char*)data;
            UNUSED(debug2String);

            break;
        case MSG_DEBUG_FD:
            if (length > 64) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            char* debugFdString = (char*)data;
            UNUSED(debugFdString);

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
            if(ACUError(acuMsgTwo) && (errorFlagBitsCan == 0 || errorFlagBitsCan == 2)){
                errorFlagBitsCan += 1;
            }
            
            else if(!ACUError(acuMsgTwo) && (errorFlagBitsCan == 1 || errorFlagBitsCan == 3)){
                errorFlagBitsCan -= 1;
            }

            if (ACUWarning(acuMsgTwo))
            {
                globalStatus.PowerLevelTorqueMap = (globalStatus.PowerLevelTorqueMap << 4 >> 4) | (1 << 7);
                writeMessage(1, MSG_DEBUG_2_0, GR_ALL, (uint8_t*)"UnderVol", 8); // Until they figure out how they want to talk to us...
            }



            //Error handling and leaving error state
            if ((errorFlagBitsCan || getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 0) == 0x1) && globalStatus.TractiveSystemVoltage >= 60)
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }
            else if(errorFlagBitsCan || getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 0) == 0x1)
            {
                globalStatus.ECUState = ERRORSTATE;
            }
            else if (globalStatus.ECUState == ERRORSTATE || (globalStatus.ECUState == TS_DISCHARGE_OFF && globalStatus.TractiveSystemVoltage < 60))
            {
                globalStatus.ECUState = GLV_ON;
            }
            


            switch(globalStatus.ECUState){
                case GLV_ON:
                    break;
                case PRECHARGE_ENGAGED:
                    if(getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 1) == 0x1){
                        globalStatus.ECUState = PRECHARGING;
                    }
                    break;
                case PRECHARGING:
                    if(getBits(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 0, 4) == 0x07){
                        globalStatus.ECUState = PRECHARGE_COMPLETE;
                    }
                    //don't add a break here
                default:
                    if(getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 1) == 0x0 || getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 2) == 0x00 && globalStatus.ECUState != PRECHARGING){
                        globalStatus.ECUState = TS_DISCHARGE_OFF;
                    }
            }
            //If ACU software latch ever opens or IR- ever opens while IR+ is closed, something has gone wrong
            if(getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 3) == 0x00 || getBits(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 1, 2) == 0x01)
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }
/*
            // IR- -> 1 = ACU Precharge Confirmation
            if((globalStatus.ECUState == PRECHARGE_ENGAGED) && (getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 1) == 0x1))
            {
                globalStatus.ECUState = PRECHARGING;
            }

            // If IR- ever becomes 0 while not in GLV_ON or PRECHARGE_ENGAGED, that is a precharge cancellation and it must start discharging.
            if(getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 1) == 0x0 && (globalStatus.ECUState != GLV_ON) && (globalStatus.ECUState != PRECHARGE_ENGAGED))
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }

            // If it is precharging with IR- closed and then IR+ goes closed as well without precharge error, precharge is complete (success confirmation)
            // IR+ -> 1 is precharge success confirmation
            if((getBits(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 0, 4) == 0x07) && (globalStatus.ECUState == PRECHARGING))
            {
                globalStatus.ECUState = PRECHARGE_COMPLETE;
            }

            //If IR+ ever opens on or after the precharging complete state, start discharging
            if(getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 2) == 0x00 && globalStatus.ECUState != GLV_ON && globalStatus.ECUState != PRECHARGE_ENGAGED && globalStatus.ECUState != PRECHARGING)
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }

            //If ACU software latch ever opens or IR- ever opens while IR+ is closed, something has gone wrong
            if(getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 3) == 0x00 || getBits(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 1, 2) == 0x01)
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }
*/
            // USE ACUWarning(acuMsgTwo) HERE FOR DASH WARNINGS
            break;
        case MSG_ACU_STATUS_3:
            if (length != 8) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            ACU_Status_MsgThree* acuMsgThree = (ACU_Status_MsgThree*)data;
            UNUSED(acuMsgThree);

            break;

        case MSG_DC_DC_STATUS:
            if (length != 8) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Msg_Lv_Dc_Dc_Status* msgLv = (Msg_Lv_Dc_Dc_Status*)data;
            UNUSED(msgLv);
            
            break;
        case MSG_INVERTER_STATUS_1:
            if (length != 6) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Inverter_Status_Msg_One* msgGriOne = (Inverter_Status_Msg_One*)data;
            UNUSED(msgGriOne);

            break;
        case MSG_INVERTER_STATUS_2:
            if (length != 6) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Inverter_Status_Msg_Two* msgGriTwo = (Inverter_Status_Msg_Two*)data;
            UNUSED(msgGriTwo);

            break;
        case MSG_INVERTER_STATUS_3:
            if (length != 3) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Inverter_Status_Msg_Three* msgGriThree = (Inverter_Status_Msg_Three*)data;

            globalStatus.VehicleSpeed = (globalStatus.RRWheelRPM + globalStatus.RLWheelRPM) * 3.141592653589 * 16.0 / 2.0 / 3.55 / 1056.0;  // FIXME: Fix the math, copied from VDM-24...

            if(GRIError(msgGriThree) && (errorFlagBitsCan == 0 || errorFlagBitsCan == 1)){
                errorFlagBitsCan += 2;
            }

            else if(!GRIError(msgGriThree) && (errorFlagBitsCan == 2 || errorFlagBitsCan == 3)){
                errorFlagBitsCan -= 2;
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

            Dash_Status_Msg *dashStatusMsg = (Dash_Status_Msg*)data;
            bool ts_on = dashStatusMsg->TSButtonData < 0;
            bool rtd = dashStatusMsg->RTDButtonData < 0;

            HAL_GPIO_WritePin(RTD_CONTROL_GPIO_Port, RTD_CONTROL_Pin, rtd);

            switch(globalStatus.ECUState){
                case GLV_ON:
                    if(ts_on){
                        globalStatus.ECUState = PRECHARGE_ENGAGED;
                    }
                    break;
                case PRECHARGE_ENGAGED:
                    if(!ts_on){
                        globalStatus.ECUState = GLV_ON;
                    }
                    break;
                case DRIVE_STANDBY:
                    if(!rtd){
                        globalStatus.ECUState = PRECHARGE_COMPLETE;
                    }
                    //don't add a break here
                case PRECHARGE_COMPLETE:
                    if(rtd && analogRead(BRAKE_F_SIGNAL) > 100 && analogRead(BRAKE_R_SIGNAL) > 100){
                        globalStatus.ECUState = DRIVE_STANDBY;
                    }
                    //don't add a break here
                default:
                    if(!ts_on){
                        globalStatus.ECUState = TS_DISCHARGE_OFF;
                    }
            }
/*
            if (globalStatus.ECUState == GLV_ON)
            {
                if(ts_on)
                {
                    globalStatus.ECUState = PRECHARGE_ENGAGED;
                }
            }            
            else if (!ts_on && globalStatus.ECUState == PRECHARGE_ENGAGED)
            {
                globalStatus.ECUState = GLV_ON;
            }
            else if (!ts_on && globalStatus.ECUState != ERRORSTATE) // If it is not in GLV_ON, PRECHARGE_ENGAGED or ERRORSTATE, if ts_off is ever true it must go to discharge
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }
            else if(globalStatus.ECUState == PRECHARGE_COMPLETE && rtd && analogRead(BRAKE_F_SIGNAL) > 100 && analogRead(BRAKE_R_SIGNAL) > 100)
            {
                globalStatus.ECUState = DRIVE_STANDBY;
            }
            else if(globalStatus.ECUState == DRIVE_STANDBY && !rtd)
            {
                globalStatus.ECUState = PRECHARGE_COMPLETE;
            }
*/
            break;

        case MSG_FAN_STATUS:
            if (length != 5) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Fan_Status_Msg* msgFan = (Fan_Status_Msg*)data;
            UNUSED(msgFan);

            break;

        case MSG_STEERING_STATUS:
            if (length != 4) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Steering_Status_Msg* msgSteer = (Steering_Status_Msg*)data;
            globalStatus.PowerLevelTorqueMap = msgSteer->Current_Torque_Map_Encoder;

            /* Do not write to these values elsewhere! */
            globalSteeringStatusRegen = msgSteer->regen;
            globalSteeringStatusButtonMap = msgSteer->buttonMap;

            // Handle buttons / regen here
    }
}
