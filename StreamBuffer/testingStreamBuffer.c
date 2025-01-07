#include "streamBuffer.h"

int main()
{
	printf("Prepare to be amazed:\n");
	addToBuffer("testingthisout!", 16, 'r');
	printBuffer();

	updateHardwareBuffer('r');
	printBuffer();

	addToBuffer("idonotknowhowtospellanything", 28, 't');
	printBuffer();

	updateHardwareBuffer('t');
	printBuffer();

	resetSoftwareBuffer('r');
	printBuffer();

	updateHardwareBuffer('t');
	printBuffer();

	updateHardwareBuffer('t');
	printBuffer();

	updateHardwareBuffer('t');
	printBuffer();

	addToBuffer("cheese", 7, 'r');
	addToBuffer("pizza", 6,'r');
	printBuffer();
}
