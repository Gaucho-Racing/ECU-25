#include "CANdler.h"
#include "stateMachine.h"
#include "msgIDs.h"
#include <stdbool.h>


void handleCANMessage(uint16_t msgID, uint8_t srcID, uint8_t *data, uint32_t length, uint32_t timestamp) {
    switch(msgID) {
        case MSG_DEBUG:
            break;
        case MSG_PING:
            break;
        case MSG_ACU_STATUS:
            break;
        case MSG_ACU_CELL_DATA_1:
            break;
        case MSG_ACU_CELL_DATA_2:
            break;
        case MSG_ACU_CELL_DATA_3:
            break;
        case MSG_ACU_CELL_DATA_4:
            break;
        case MSG_ACU_CELL_DATA_5:
            break;
        case MSG_LV_DC_DC_STATUS:
            break;
        case MSG_DTI_INVERTER_STATUS:
            break;
        case MSG_GR_INVERTER_STATUS:
            break;
        case MSG_FAN_STATUS:
            break;


        /* update globals in stateMachine */
    }
}
