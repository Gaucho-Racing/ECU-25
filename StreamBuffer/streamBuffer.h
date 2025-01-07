#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define HARDWARE_SIZE 8
#define SOFTWARE_SIZE 512

uint8_t getSoftwareBufferSize(char rt);
void addToBuffer(uint8_t *addBuffer, uint8_t size, char rt);
void updateHardwareBuffer(char rt);
void resetSoftwareBuffer(char rt);
void printBuffer();
