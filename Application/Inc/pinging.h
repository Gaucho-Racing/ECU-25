#ifndef PINGING_H
#define PINGING_H

#include <stdint.h>

#define PINGTIMEOUT 250 // timeout time for pings in ms
#define PINGCOUNT 11    // How many pinging objects there are

extern const uint8_t pingIDs[PINGCOUNT]; // GR IDs of devices that are pinged
extern uint32_t pingTimes[PINGCOUNT]; // round-trip ping times of said devices

/**
Ping Schedule

Sends a ping if there is a need to, manages missing ping responses.

Can call as often as desired, will quite if nothing needed.
*/
void pingSchedule(void);

/**
Respond To Ping

@param destID Desintation GR ID
@param timestamp When the interrupt was recieved

Handles ping responses and updates internal setup.
Not dissimilar to CANDler operation but specific to pings.
*/
void respondToPing(uint8_t destID, uint32_t timestamp);

#endif // PINGING_H
