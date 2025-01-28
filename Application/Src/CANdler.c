#include "CANdler.h"
#include "stateMachine.h"
#include "msgIDs.h"
#include <stdbool.h>

typedef struct {    // Since it is so big it would be nice to have a struct handle it instead of manually going through it
    uint8_t Accumulator_Voltage;
    uint8_t TS_Voltage;
    uint8_t Accumulator_Current;
    uint8_t Accumulator_SOC;
    uint8_t GLV_SOC;
    uint8_t V_20v_Voltage;
    uint8_t V_12v_Voltage;
    uint8_t SDC_Voltage;
    uint8_t Min_Cell_Voltage;
    uint8_t Max_Cell_Temp;
    uint8_t Over_Temp_Error;
    uint8_t Over_Voltage_Error;
    uint8_t Under_Voltage_Error;
    uint8_t Over_Current_Error;
    uint8_t Under_Current_Error;
    uint8_t Under_Voltage_20v_Warning;
    uint8_t Under_Voltage_12v_Warning;
    uint8_t Under_Voltage_SDC_Warning;
    uint8_t Precharge_Error;
    uint8_t IRminus_Precharge_State;
    uint8_t IRplus_State;
    uint8_t Software_Latch;
    uint8_t HV_Input_Voltage;
    uint8_t HV_Output_Voltage;
    uint8_t HV_Input_Current;
    uint8_t HV_Output_Current;
} ACU_Status_Msg;

void handleCANMessage(uint16_t msgID, uint8_t srcID, uint8_t *data, uint32_t length, uint32_t timestamp) {
    switch(msgID) {
        case MSG_DEBUG:
            if (length != 64) {
                /* BAD MESSAGE? */
            }

            break;
        case MSG_PING:
            if (length != 4) {
                /* BAD MESSAGE? */
            }

            break;
        case MSG_ACU_STATUS:
            if (length != 24) {
                /* BAD MESSAGE? */
            }

            // Should probably double check this approach because we get a lot of data that we do not store anywhere, including error flags
            ACU_Status_Msg* msg = (ACU_Status_Msg*)data;
            globalStatus.AccumulatorStateOfCharge = msg->Accumulator_SOC;
            globalStatus.GLVStateOfCharge = msg->GLV_SOC;
            globalStatus.TractiveSystemVoltage = msg->TS_Voltage;
    
            break;
        case MSG_ACU_CELL_DATA_1:
            if (length != 64) {
                /* BAD MESSAGE? */
            }
            
            break;
        case MSG_ACU_CELL_DATA_2:
            if (length != 64) {
                /* BAD MESSAGE? */
            }
            
            break;
        case MSG_ACU_CELL_DATA_3:
            if (length != 64) {
                /* BAD MESSAGE? */
            }
            
            break;
        case MSG_ACU_CELL_DATA_4:
            if (length != 64) {
                /* BAD MESSAGE? */
            }
            
            break;
        case MSG_ACU_CELL_DATA_5:
            if (length != 64) {
                /* BAD MESSAGE? */
            }
            
            break;
        case MSG_LV_DC_DC_STATUS:
            if (length != 8) {
                /* BAD MESSAGE? */
            }
            
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
