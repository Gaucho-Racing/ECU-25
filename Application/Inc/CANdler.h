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
    uint8_t Error_Warning_Bits;
    uint8_t Precharge_Error;
    uint8_t IR_State_Software_Latch_Bits;
    uint8_t HV_Input_Voltage;
    uint8_t HV_Output_Voltage;
    uint8_t HV_Input_Current;
    uint8_t HV_Output_Current;
} ACU_Status_Msg;

typedef struct {
    uint8_t Accumulator_Voltage;
    uint8_t TS_Voltage;
    uint8_t Accumulator_Current;
    uint8_t Accumulator_SOC;
    uint8_t GLV_SOC;
} ACU_Status_MsgOne;

typedef struct {
    
} ACU_Status_MsgTwo;

typedef struct {

} ACU_Status_MsgThree;

typedef struct {
     uint8_t Input_Voltage;
     uint8_t Output_Voltage;
     uint8_t Input_Current;
     uint8_t Output_Current;
} Msg_Lv_Dc_Dc_Status;

typedef struct {
    uint16_t AC_current;
    uint16_t DC_current;
    uint16_t DC_voltage;
    uint8_t U_MOSFET_temperature;
    uint8_t V_MOSFET_temperature;
    uint8_t W_MOSFET_temperature;
    uint8_t Water_temperature;
    uint16_t Motor_RPM;
    uint8_t Motor_temperature;
    uint8_t fault_map;
} Gr_Inverter_Status_Msg;

typedef struct {
    int32_t ERPM;
    int16_t Duty_Cycle;
    int16_t Input_Voltage;
    int16_t AC_Current;
    int16_t DC_Current;
    int16_t Controller_Temp;
    int16_t Motor_Temp;
    uint8_t Faults;
    int32_t FOC_Id;
    int32_t FOC_Iq;
    uint8_t Throttle;
    uint8_t Brake;
    uint8_t Digital_IO;
    uint32_t Drive_Enable;
    uint16_t Flags;
    uint8_t CAN_Version;
} Dti_Inverter_Status_Msg;

typedef struct {
    uint16_t Fan_Speed;
    uint8_t Input_Voltage;
    uint8_t Output_Voltage;
    uint8_t Current;
} Fan_Status_Msg;

extern uint8_t errorFlagBitsCan;

#endif // CANDLER_H
