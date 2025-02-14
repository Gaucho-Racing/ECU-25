#ifndef CANDLER_H
#define CANDLER_H

#include <stdint.h>

void handleCANMessage(uint16_t msgID, uint8_t srcID, uint8_t *data, uint32_t length, uint32_t timestamp);

typedef struct {
    uint16_t Accumulator_Voltage;
    uint16_t TS_Voltage;
    uint16_t Accumulator_Current;
    uint8_t Accumulator_SOC;
    uint8_t GLV_SOC;
} ACU_Status_MsgOne;

typedef struct {
    uint8_t V_20v_Voltage;
    uint8_t V_12v_Voltage;
    uint8_t SDC_Voltage;
    uint8_t Min_Cell_Voltage;
    uint8_t Max_Cell_Temp;
    uint8_t Error_Warning_Bits;
    uint8_t Precharge_Error;
    uint8_t IR_State_Software_Latch_Bits;
} ACU_Status_MsgTwo;

typedef struct {
    uint16_t HV_Input_Voltage;
    uint16_t HV_Output_Voltage;
    uint16_t HV_Input_Current;
    uint16_t HV_Output_Current;
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
    uint16_t Motor_Rpm;
} Inverter_Status_Msg_One;

typedef struct {
    uint8_t U_MOSFET_temperature;
    uint8_t V_MOSFET_temperature;
    uint8_t W_MOSFET_temperature;
} Inverter_Status_Msg_Two;

typedef struct {
    uint8_t Motor_temperature;
    uint8_t fault_map;
} Inverter_Status_Msg_Three;

typedef struct {
    uint16_t Fan_Speed;
    uint8_t Input_Voltage;
    uint8_t Output_Voltage;
    uint8_t Current;
} Fan_Status_Msg;

typedef struct {
    uint8_t DASH_LED;
    uint8_t Button_LED_1_R;
    uint8_t Button_LED_1_G;
    uint8_t Button_LED_1_B;
    uint8_t Button_LED_2_R;
    uint8_t Button_LED_2_G;
    uint8_t Button_LED_2_B;
} Dash_Config_Msg;

typedef struct {
    uint8_t Current_Torque_Map_Encoder;
    uint8_t regen;
    uint8_t buttonMap;
} Steering_Status_Msg;

typedef struct {
    uint8_t Led_Map;
    int8_t TSButtonData;
    int8_t RTDButtonData;
} Dash_Status_Msg;

extern uint8_t errorFlagBitsCan;

#endif // CANDLER_H
