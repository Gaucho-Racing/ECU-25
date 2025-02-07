#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#define TICK_FREQ 100000 // overflow after 12 hrs runtime teehee

uint32_t millis(void);
uint8_t getBit(uint8_t number, uint8_t indexFromRight);
uint8_t get2Bits(uint8_t number, uint8_t indexFromRight);
uint8_t convertStatusLump(StatusLump lump);
void setSoftwareLatch(uint8_t close);

typedef struct {
    uint8_t ECUState;
    uint8_t StatusBits[3];
    uint8_t PowerLevelTorqueMap;
    uint8_t MaxCellTemp;
    uint8_t AccumulatorStateOfCharge;
    uint8_t GLVStateOfCharge;
} ECUStatusOne;

typedef struct {
    uint16_t TractiveSystemVoltage;
    uint16_t VehicleSpeed;
    uint16_t FRWheelRPM;
    uint16_t FLWheelRPM;
} ECUStatusTwo;

typedef struct {
    uint16_t RRWheelRPM;
    uint16_t RLWheelRPM;
} ECUStatusThree;

#endif // UTILS_H
