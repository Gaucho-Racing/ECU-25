#ifndef PINGING_H
#define PINGING_H

#include <stdint.h>

#define PINGTIMEOUT 250
#define PINGCOUNT 11

extern const uint8_t pingIDs[PINGCOUNT]; // GR IDs of devices that are pinged
extern uint32_t pingTimes[PINGCOUNT]; // round-trip ping times of said devices
void pingSchedule(void);

#endif // PINGING_H
