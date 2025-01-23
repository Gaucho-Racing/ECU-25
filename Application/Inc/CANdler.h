#ifndef CANDLER_H
#define CANDLER_H

#include <stdint.h>

void handleCANMessage(uint16_t msgID, uint8_t srcID, uint8_t *data, uint32_t length, uint32_t timestamp);

#endif // CANDLER_H
