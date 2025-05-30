#include <stdint.h>
#include <stdbool.h>

#include "utils.h"
#include "inverter.h"
#include "fdcan.h"
#include "msgIDs.h"
#include "stateMachine.h"
#include "customIDs.h"

volatile InverterSettings globalInverterSettings[3] = {0};

volatile DTI_Data globalInverterData = {0};

volatile int32_t lastInverterPingMillis = BAD_TIME_Negative1;

void sendInverterCommand(void)
{
    if (millis() - lastInverterPingMillis >= INVERTER_MINIMUM_SENDING_TIME_MS) // Must send every 10 ms for power
    {
        lastInverterPingMillis = millis();

        writeDtiMessage(MSG_DTI_CONTROL_12, (uint8_t*)&globalInverterSettings[0].driveEnable, 1);
        writeDtiMessage(MSG_DTI_CONTROL_5, (uint8_t*)&globalInverterSettings[0].acCurrent, 2);

        writeMessage(PrimaryBusCAN, MSG_INVERTER_COMMAND, GR_GR_INVERTER_1, (uint8_t*)&globalInverterSettings[1], 7);
        writeMessage(PrimaryBusCAN, MSG_INVERTER_COMMAND, GR_GR_INVERTER_2, (uint8_t*)&globalInverterSettings[2], 7);
        /*
        LOGOMATIC("Global Inverter Settings 1 -- Set AC Current %d\n", globalInverterSettings[0].acCurrent);
        LOGOMATIC("Global Inverter Settings 1 -- Drive Enable %d\n", globalInverterSettings[0].driveEnable);
        LOGOMATIC("Global Inverter Settings 2 -- Set AC Current %d\n", globalInverterSettings[1].acCurrent);
        LOGOMATIC("Global Inverter Settings 2 -- Set DC Current %d\n", globalInverterSettings[1].dcCurrent);
        LOGOMATIC("Global Inverter Settings 2 -- RPM Limit %d\n", globalInverterSettings[1].rpmLimit);
        LOGOMATIC("Global Inverter Settings 2 -- Drive Enable %d\n", globalInverterSettings[1].driveEnable);
        LOGOMATIC("Global Inverter Settings 3 -- Set AC Current %d\n", globalInverterSettings[2].acCurrent);
        LOGOMATIC("Global Inverter Settings 3 -- Set DC Current %d\n", globalInverterSettings[2].dcCurrent);
        LOGOMATIC("Global Inverter Settings 3 -- RPM Limit %d\n", globalInverterSettings[2].rpmLimit);
        LOGOMATIC("Global Inverter Settings 3 -- Drive Enable %d\n", globalInverterSettings[2].driveEnable);
        */
    }
}

void controlInverters(bool driveEnable)
{
    globalInverterSettings[0].driveEnable = (uint8_t)driveEnable;
    globalInverterSettings[1].driveEnable = (uint8_t)driveEnable;
    globalInverterSettings[2].driveEnable = (uint8_t)driveEnable;

    sendInverterCommand();
}
