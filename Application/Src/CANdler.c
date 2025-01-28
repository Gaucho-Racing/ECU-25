#include <stdbool.h>

#include "CANdler.h"
#include "stateMachine.h"
#include "msgIDs.h"
#include "pinging.h"

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

            ACU_Status_Msg* msg = (ACU_Status_Msg*)data;
            globalStatus.AccumulatorStateOfCharge = msg->Accumulator_SOC;
            globalStatus.GLVStateOfCharge = msg->GLV_SOC;
            globalStatus.TractiveSystemVoltage = msg->TS_Voltage;
            globalStatus.MaxCellTemp = msg->Max_Cell_Temp;
    
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

            Msg_Lv_Dc_Dc_Status* msg = (Msg_Lv_Dc_Dc_Status*)data;
            UNUSED(msg);

            break;
        case MSG_DTI_INVERTER_STATUS:
            if (length != 32) {
                /* BAD MESSAGE? */
            }

            
            
            break;
        case MSG_GR_INVERTER_STATUS:
            if (length != 19) {
                /* BAD MESSAGE? */
            }

            break;
        case MSG_FAN_STATUS:
            if (length != 5) {
                /* BAD MESSAGE? */
            }

            break;
        /* update globals in stateMachine */
    }
}
