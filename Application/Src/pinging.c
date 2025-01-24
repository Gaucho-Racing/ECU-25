#include "stdint.h"
#include "stm32g4xx_hal.h"
#include "pinging.h"
#include "fdcan.h"

// void writeMessage(uint8_t bus, uint16_t msgID, uint8_t destID, uint8_t data[], uint32_t len)

void pingSchedule()
{
    uint8_t bus;
    uint16_t msgID;
    uint8_t destID;
    uint8_t* data;
    uint32_t len;

    uint32_t ms = HAL_GetTick();
    switch ((HAL_GetTick() % 250 / 23)) {
        case 0:
            bus = ;
            msgID = ;
            destID = ;
            data = ;
            len = ;
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        case 10:
            break;
    }

    writeMessage(bus, msgID, destID, data, len);
}