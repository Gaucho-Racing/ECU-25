#ifndef CANDLER_H
#define CANDLER_H

#include <stdint.h>

void handleCANMessage(uint16_t msgID, uint8_t srcID, uint8_t *data, uint32_t length, uint32_t timestamp);

typedef struct {
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

// Bad very not good code
typedef struct {
    uint8_t Cell_0_Voltage;
    uint8_t Cell_0_Temp;
    uint8_t Cell_30_Temp;
    uint8_t Cell_31_Voltage;
    uint8_t Cell_31_Temp;

} MSG_ACU_CELL_DATA_1;

typedef struct {
    uint8_t Cell_32_Voltage;
    uint8_t Cell_32_Temp;
    uint8_t Cell_33_Voltage;
    uint8_t Cell_63_Temp;

} MSG_ACU_CELL_DATA_2;

typedef struct {
    uint8_t Cell_64_Voltage;
    uint8_t Cell_64_Temp;
    uint8_t Cell_94_Temp;
    uint8_t Cell_95_Voltage;
    uint8_t Cell_95_Temp;

} MSG_ACU_CELL_DATA_3;

typedef struct {
    uint8_t Cell_95_Voltage_1;
    uint8_t Cell_64_Temp;
    uint8_t Cell_95_Voltage_2; // there's a duplicate for some reason; don't ask me
    uint8_t Cell_95_Temp;

} MSG_ACU_CELL_DATA_4;

typedef struct {
    uint8_t Cell_64_Voltage;
    uint8_t Cell_64_Temp;
    uint8_t Cell_95_voltage;
    uint8_t Cell_95_Temp;

} MSG_ACU_CELL_DATA_5;

#endif // CANDLER_H
