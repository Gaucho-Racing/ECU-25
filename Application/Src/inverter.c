#include <stdint.h>
#include <stdbool.h>

#include "utils.h"
#include "inverter.h"
#include "fdcan.h"
#include "msgIDs.h"

volatile DTI_Data globalInverterData = {0};

volatile InverterSettings globalInverterSettings[3] = { // Set to 4 iff fourth motor/inverter
    {0, .RPM_Limit = 1, 0},
    {0, .RPM_Limit = 1, 0},
    {0, .RPM_Limit = 1, 0},
//  {0, .RPM_Limit = 1, 0}, // Enable iff fourth motor/inverter
};

volatile int32_t lastInverterPingMillis = -1;

void sendInverterCommand(void)
{
    if (millis() - lastInverterPingMillis >= INVERTER_MINIMUM_SENDING_TIME_MS) // Must send every 100 ms for power
    {
        lastInverterPingMillis = millis();
        writeMessage(1, MSG_INVERTER_COMMAND, GR_GR_INVERTER_1, (uint8_t*)&globalInverterSettings[0], 7);
        writeMessage(1, MSG_INVERTER_COMMAND, GR_GR_INVERTER_2, (uint8_t*)&globalInverterSettings[1], 7);
        writeMessage(1, MSG_INVERTER_COMMAND, GR_GR_INVERTER_3, (uint8_t*)&globalInverterSettings[2], 7);
//      writeMessage(1, MSG_INVERTER_COMMAND, GR_GR_INVERTER_4, (uint8_t*)&globalInverterSettings[3], 7);   // Enable iff fourth motor/inverter
    }
}

void controlInverters(bool driveEnable)
{
    globalInverterSettings[0] = (InverterSettings){0, 0, 0, (uint8_t)driveEnable};
    globalInverterSettings[1] = (InverterSettings){0, 0, 0, (uint8_t)driveEnable};
    globalInverterSettings[2] = (InverterSettings){0, 0, 0, (uint8_t)driveEnable};
//  globalInverterSettings[3] = (InverterSettings){0, 0, 0, (uint8_t)driveEnable};  // Enable iff fourth motor/inverter

    sendInverterCommand();
}
