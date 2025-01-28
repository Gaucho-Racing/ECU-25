#include <stdbool.h>

#include "CANdler.h"
#include "stateMachine.h"
#include "msgIDs.h"
#include "pinging.h"

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
                /* BAD MESSAGE? */
            }

            char* string = (char*)data;
            UNUSED(string);

            break;
        case MSG_PING:
            if (length != 4) {
                /* BAD MESSAGE? */
            }

            respondToPing(srcID, *(uint32_t*)data);

            break;
        case MSG_ACU_STATUS:
            if (length != 24) {
                /* BAD MESSAGE? */
            }

            ACU_Status_Msg* msgAcu = (ACU_Status_Msg*)data;
            globalStatus.AccumulatorStateOfCharge = msgAcu->Accumulator_SOC;
            globalStatus.GLVStateOfCharge = msgAcu->GLV_SOC;
            globalStatus.TractiveSystemVoltage = msgAcu->TS_Voltage;
            globalStatus.MaxCellTemp = msgAcu->Max_Cell_Temp;
    
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
                /* BAD MESSAGE? */
            }

            Msg_Lv_Dc_Dc_Status* msgLv = (Msg_Lv_Dc_Dc_Status*)data;
            UNUSED(msgLv);

            break;
        case MSG_DTI_INVERTER_STATUS:
            if (length != 32) {
                /* BAD MESSAGE? */
            }

            Dti_Inverter_Status_Msg* msgDti = (Dti_Inverter_Status_Msg*)data;
            UNUSED(msgDti);

            break;
        case MSG_GR_INVERTER_STATUS:    // THIS WILL NEED TO BE REWORKED EXTENSIVELY
            if (length != 19) {
                /* BAD MESSAGE? */
            }

            Msg_Gr_Inverter_Status* msgGri = (Msg_Gr_Inverter_Status*)data;
            globalStatus.inverters[0].ACCurrent = msgGri->AC_Current;
            globalStatus.inverters[0].Temp = findTernaryMax(msgGri->U_MOSFET_Temperature, msgGri->V_MOSFET_Temperature, msgGri->W_MOSFET_Temperature);
            globalStatus.inverters[0].RPM = msgGri->Motor_RPM;
            globalStatus.inverters[1] = globalStatus.inverters[0];
            globalStatus.inverters[2] = globalStatus.inverters[1];
            globalStatus.inverters[3] = globalStatus.inverters[2];

            globalStatus.RLWheelRPM = globalStatus.inverters[0].RPM;
            globalStatus.RRWheelRPM = globalStatus.inverters[1].RPM;
            globalStatus.FLWheelRPM = globalStatus.inverters[2].RPM;
            globalStatus.FRWheelRPM = globalStatus.inverters[3].RPM;
            
            break;
        case MSG_FAN_STATUS:
            if (length != 5) {
                /* BAD MESSAGE? */
            }
            Fan_Status* msgFan = (Fan_Status*)data;

            break;
        /* update globals in stateMachine */
    }
}