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
                /* BAD MESSAGE? */
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            respondToPing(srcID, *(uint32_t*)data);

            break;
        case MSG_ACU_STATUS:
            if (length != 24) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            ACU_Status_Msg* msgAcu = (ACU_Status_Msg*)data;
            globalStatus.AccumulatorStateOfCharge = msgAcu->Accumulator_SOC;
            globalStatus.GLVStateOfCharge = msgAcu->GLV_SOC;
            globalStatus.TractiveSystemVoltage = msgAcu->TS_Voltage;
            globalStatus.MaxCellTemp = msgAcu->Max_Cell_Temp;

            //errorFlagBitsCan logic
            if(msgAcu->Error_Warning_Bits != 0x00 && (errorFlagBitsCan == 0 || errorFlagBitsCan == 2)){
                errorFlagBitsCan += 1;
            }
            else if(msgAcu->Error_Warning_Bits == 0x00 && (errorFlagBitsCan == 1 || errorFlagBitsCan == 3)){
                errorFlagBitsCan -= 1;
            }

            //Error handling and leaving error state
            if ((errorFlagBitsCan || msgAcu->Precharge_Error == 0b1) && globalStatus.TractiveSystemVoltage >= 60)
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }
            else if(errorFlagBitsCan || msgAcu->Precharge_Error == 0b1){
                globalStatus.ECUState = ERRORSTATE;
            }
            else if (globalStatus.ECUState == ERRORSTATE || globalStatus.ECUState == TS_DISCHARGE_OFF && globalStatus.TractiveSystemVoltage < 60)
            {
                globalStatus.ECUState = GLV_ON;
            }
            
            // IR- -> 1 = ACU Precharge Confirmation
            if(globalStatus.ECUState == PRECHARGE_ENGAGED && getBit(msgAcu->IR_State_Software_Latch_Bits, 0) == 0b1){
                globalStatus.ECUState = PRECHARGING;
            }

            // If IR- ever becomes 0 while not in GLV_ON or PRECHARGE_ENGAGED, that is a precharge cancellation and it must start discharging.
            if(getBit(msgAcu->IR_State_Software_Latch_Bits, 0) == 0b0 && globalStatus.ECUState != GLV_ON && globalStatus.ECUState != PRECHARGE_ENGAGED)
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }

            // If it is precharging with IR- closed and then IR+ goes closed as well, precharge is complete (success confirmation)
            // IR+ -> 1 is precharge success confirmation
            if(getBit(msgAcu->IR_State_Software_Latch_Bits, 0) == 0b1 && getBit(msgAcu->IR_State_Software_Latch_Bits, 1) == 0b1 && globalStatus.ECUState == PRECHARGING)
            {
                globalStatus.ECUState = PRECHARGE_COMPLETE;
            }

            //If IR+ ever opens on or after the precharging complete state, start discharging
            if(getBit(msgAcu->IR_State_Software_Latch_Bits, 1) == 0b0 && globalStatus.ECUState != GLV_ON && globalStatus.ECUState != PRECHARGE_ENGAGED && globalStatus.ECUState != PRECHARGING)
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }
            break;

        case MSG_LV_DC_DC_STATUS:
            if (length != 8) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Msg_Lv_Dc_Dc_Status* msgLv = (Msg_Lv_Dc_Dc_Status*)data;
            
            break;
        case MSG_DTI_INVERTER_STATUS:
            if (length != 32) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Dti_Inverter_Status_Msg* msgDti = (Dti_Inverter_Status_Msg*)data;
            //used for torque vectoring; outside of scope
            uint8_t throttle = msgDti->Throttle;
            break;
        case MSG_GR_INVERTER_STATUS:    // THIS WILL NEED TO BE REWORKED EXTENSIVELY
            if (length != 19) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Gr_Inverter_Status_Msg* msgGri = (Gr_Inverter_Status_Msg*)data;
            globalStatus.inverters[0].ACCurrent = msgGri->AC_current;
            globalStatus.inverters[0].Temp = findTernaryMax(msgGri->U_MOSFET_temperature, msgGri->V_MOSFET_temperature, msgGri->W_MOSFET_temperature);
            globalStatus.inverters[0].RPM = msgGri->Motor_RPM;
            globalStatus.inverters[1] = globalStatus.inverters[0];
            globalStatus.inverters[2] = globalStatus.inverters[1];
            globalStatus.inverters[3] = globalStatus.inverters[2];

            globalStatus.RLWheelRPM = globalStatus.inverters[0].RPM;
            globalStatus.RRWheelRPM = globalStatus.inverters[1].RPM;
            globalStatus.FLWheelRPM = globalStatus.inverters[2].RPM;
            globalStatus.FRWheelRPM = globalStatus.inverters[3].RPM;

            globalStatus.VehicleSpeed = (globalStatus.RRWheelRPM + globalStatus.RLWheelRPM) * 3.141592653589 * 8 / 3.55 / 1056.0;  // Probably fix this...

            if(msgGri->fault_map == 0x00 && (errorFlagBitsCan == 2 || errorFlagBitsCan == 3)){
                errorFlagBitsCan -= 2;
            }

            else if(msgGri->fault_map != 0x00 && (errorFlagBitsCan == 0 || errorFlagBitsCan == 1)){
                errorFlagBitsCan += 2;
            }

            if (errorFlagBitsCan && globalStatus.TractiveSystemVoltage >= 60)
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }
            else if(errorFlagBitsCan){
                globalStatus.ECUState = ERRORSTATE;
            }
            else if(globalStatus.ECUState == ERRORSTATE || globalStatus.ECUState == TS_DISCHARGE_OFF && globalStatus.TractiveSystemVoltage < 60) 
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

        case MSG_DASH_CONFIG:
            if (length != 7) {
                numberOfBadMessages++;
                return;
            }
            else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

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
            if (length != 2) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }
            

            steerSettings.buttonFlags |= data[0];
            steerSettings.buttonFlags &= data[1];

            // steering stauts msg parsing

    }
}
