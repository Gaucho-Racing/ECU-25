#include "streamBuffer.h"

uint8_t hardwareRxBuffer[HARDWARE_SIZE];
uint8_t hardwareTxBuffer[HARDWARE_SIZE];

uint8_t softwareRxBuffer[SOFTWARE_SIZE];
uint8_t softwareRxBufferSize = 0;

uint8_t softwareTxBuffer[SOFTWARE_SIZE];
uint8_t softwareTxBufferSize = 0;

/**
  * @brief Tells you the size of rx or tx
  * @retval None
  */
uint8_t getSoftwareBufferSize(char rt)
{
	if (rt == 'r') {
		return softwareRxBufferSize;
	} else if (rt == 't') {
		return softwareTxBufferSize;
	}
}

/**
  * @brief Write to either rx or tx buffer depending on rt
  * @retval None
  */
void addToBuffer(uint8_t *addBuffer, uint8_t size, char rt)
{
	if (rt == 'r') {
		while (size + softwareRxBufferSize >= SOFTWARE_SIZE);

		memcpy(&softwareRxBuffer[softwareRxBufferSize], addBuffer, size);
		softwareRxBufferSize += size;
	} else if (rt == 't') {
		while (size + softwareTxBufferSize >= SOFTWARE_SIZE);

		memcpy(&softwareTxBuffer[softwareTxBufferSize], addBuffer, size);
		softwareTxBufferSize += size;
	}
}

/**
  * @brief Updates hardware buffers
  * @retval None
  */
void updateHardwareBuffer(char rt)
{
	if (rt == 'r') {
		memcpy(hardwareRxBuffer, softwareRxBuffer, HARDWARE_SIZE - 1);

		for (int i = 0; i <= SOFTWARE_SIZE - HARDWARE_SIZE; i++) {
			softwareRxBuffer[i] = softwareRxBuffer[HARDWARE_SIZE + i - 1];
		}

		softwareRxBufferSize -= (HARDWARE_SIZE - 1);
	} else if (rt == 't') {
		memcpy(hardwareTxBuffer, softwareTxBuffer, HARDWARE_SIZE - 1);

		for (int i = 0; i <= SOFTWARE_SIZE - HARDWARE_SIZE; i++) {
			softwareTxBuffer[i] = softwareTxBuffer[HARDWARE_SIZE + i - 1];
		}
		
		softwareTxBufferSize -= (HARDWARE_SIZE - 1);
	}
}


/**
  * @brief Sets buffer size to 0 to clear buffer
  * @retval None
  */
void resetSoftwareBuffer(char rt)
{
	if (rt == 'r') {
		softwareRxBufferSize = 0;
	} else if (rt == 't') {
		softwareTxBufferSize = 0;
	}
}

/**
  * @brief Prints the buffer
  * @retval None
  */
void printBuffer()
{
	printf("\nRx Software: ");
	for (int i = 0; i < softwareRxBufferSize; i++) {
		printf("%c", softwareRxBuffer[i]);
	}

	printf("\nRx Hardware: ");

	for (int i = 0; i < HARDWARE_SIZE; i++) {
		printf("%c", hardwareRxBuffer[i]);
	}

	printf("\nTx Software: ");

	for (int i = 0; i < softwareTxBufferSize; i++) {
		printf("%c", softwareTxBuffer[i]);
	}

	printf("\nTx Hardware: ");

	for (int i = 0; i < HARDWARE_SIZE; i++) {
		printf("%c", hardwareTxBuffer[i]);
	}

	printf("\n");
}
