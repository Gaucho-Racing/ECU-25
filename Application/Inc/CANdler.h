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

typedef struct {
     uint8_t Input_Voltage;
     uint8_t Output_Voltage;
     uint8_t Input_Current;
     uint8_t Output_Current;
} Msg_Lv_Dc_Dc_Status;

typedef struct {
    uint16_t Fan_Speed;
    uint8_t Input_Voltage;
    uint8_t Output_Voltage;
    uint8_t Current;
} Fan_Status_Msg;

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
    uint8_t fault_map[8];
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
    uint8_t Digital_IO_Input[4];
    uint8_t Digital_IO_Output[4];
    uint32_t Drive_Enable;
    uint16_t Flags;
    uint8_t CAN_Version;
} Dti_Inverter_Status_Msg;

typedef struct {
    uint16_t AC_Current;
    uint16_t DC_Current;
    uint16_t DC_Voltage;
    uint8_t U_MOSFET_Temperature;
    uint8_t V_MOSFET_Temperature;
    uint8_t W_MOSFET_Temperature;
    uint8_t Water_Temperature;
    uint16_t Motor_RPM;
    uint8_t Motor_Temperature;
    bool Over_Voltage_Fault;
    bool Under_Voltage_Fault;
    bool Inv_Overtemp_Fault;
    bool Motor_Overtemp_Fault;
    bool Transistor_Fault;
    bool Encoder_Fault;
    bool CAN_Fault;

} Msg_Gr_Inverter_Status;

typedef struct {
    uint16_t Fan_Speed;
    uint8_t Input_Voltage;
    uint8_t Output_Voltage;
    uint8_t Current;
} Fan_Status;

#endif // CANDLER_H
