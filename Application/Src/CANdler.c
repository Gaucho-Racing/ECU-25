#include <stdbool.h>

#include "CANdler.h"
#include "stateMachine.h"
#include "msgIDs.h"
#include "pinging.h"
#include "adc.h"

uint8_t getBit(uint8_t number, uint8_t indexFromRight)
{
    return (number >> (7 - indexFromRight)) & 0b1;
}

uint8_t get2Bits(uint8_t number, uint8_t indexFromRight)
{
    return (number >> (7 - indexFromRight)) & 0b11;
}

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

            if (msgAcu->Error_Warning_Bits != 0xFF)
            {
                globalStatus.ECUState = ERRORSTATE;
            }

            if (msgAcu->Precharge_Error == 0b1)            
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }

            if (msgAcu->Error_Warning_Bits == 0xFF && globalStatus.ECUState == ERRORSTATE)
            {
                globalStatus.ECUState = GLV_ON;
            }

            break;

        // Technically we can read the cell data, but it isn't necessary for us
        // case MSG_ACU_CELL_DATA_1:
        //     if (length != 64) {
        //         /* BAD MESSAGE? */
        //     }

            
        //     break;
        // case MSG_ACU_CELL_DATA_2:
        //     if (length != 64) {
        //         /* BAD MESSAGE? */
        //     }
            
        //     break;
        // case MSG_ACU_CELL_DATA_3:
        //     if (length != 64) {
        //         /* BAD MESSAGE? */
        //     }
            
        //     break;
        // case MSG_ACU_CELL_DATA_4:
        //     if (length != 64) {
        //         /* BAD MESSAGE? */
        //     }
            
        //     break;
        // case MSG_ACU_CELL_DATA_5:
        //     if (length != 64) {
        //         /* BAD MESSAGE? */
        //     }
            
        //     break;
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

            globalStatus.VehicleSpeed = (globalStatus.RRWheelRPM + globalStatus.RLWheelRPM) * 3.141592653539 * 8 / 3.55 / 1056.0;  // Probably fix this...

            if (msgGri->fault_map != 0xFF)
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }

            break;
        case MSG_DASH_STATUS:
            if (length != 1) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

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
            
            else if (ts_off && globalStatus.ECUState == PRECHARGING){
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }

            else if (ts_off && globalStatus.ECUState == PRECHARGE_COMPLETE){
                globalStatus.ECUState = TS_DISCHARGE_OFF;
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
            if (length != 2) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            // steering stauts msg parsing

    }
}