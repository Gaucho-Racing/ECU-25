#include <stdint.h>
#include <stdbool.h>

#include "utils.h"
#include "inverter.h"
#include "fdcan.h"
#include "msgIDs.h"
#include "customIDs.h"
#include "stateMachine.h"

// Q: What is going on here DTI <-> FOCER?
// A: This whole file setup is weirdly AMK specific!
// TODO Maybe migrate some code over to here for safeguarding

volatile DTI_Data globalInverterData = {0};

volatile InverterSettings globalInverterSettings[2] = {0};

volatile int32_t lastInverterPingMillis = BAD_TIME_Negative1;

void sendInverterCommand(void)
{
    if (millis() - lastInverterPingMillis >= INVERTER_MINIMUM_SENDING_TIME_MS) // Must send every 100 ms for power
    {
        lastInverterPingMillis = millis();

        writeMessage(PrimaryBusCAN, MSG_INVERTER_COMMAND, GR_GR_INVERTER_1, (uint8_t*)&globalInverterSettings[0], 7);
        writeMessage(PrimaryBusCAN, MSG_INVERTER_COMMAND, GR_GR_INVERTER_2, (uint8_t*)&globalInverterSettings[1], 7);

        // LOGOMATIC("Global Inverter Settings 1 -- Set AC Current %d\n", globalInverterSettings[0].Set_AC_Current);
        // LOGOMATIC("Global Inverter Settings 1 -- Set DC Current %d\n", globalInverterSettings[0].Set_DC_Current);
        // LOGOMATIC("Global Inverter Settings 1 -- RPM Limit %d\n", globalInverterSettings[0].RPM_Limit);
        // LOGOMATIC("Global Inverter Settings 1 -- Drive Enable %d\n", globalInverterSettings[0].Drive_Enable);
        // LOGOMATIC("Global Inverter Settings 2 -- Set AC Current %d\n", globalInverterSettings[1].Set_AC_Current);
        // LOGOMATIC("Global Inverter Settings 2 -- Set DC Current %d\n", globalInverterSettings[1].Set_DC_Current);
        // LOGOMATIC("Global Inverter Settings 2 -- RPM Limit %d\n", globalInverterSettings[1].RPM_Limit);
        // LOGOMATIC("Global Inverter Settings 2 -- Drive Enable %d\n", globalInverterSettings[1].Drive_Enable);;
    }
}

void controlInverters(bool driveEnable)
{
    globalInverterSettings[0] = (InverterSettings){0, 0, 0, (uint8_t)driveEnable};
    globalInverterSettings[1] = (InverterSettings){0, 0, 0, (uint8_t)driveEnable};

    sendInverterCommand();
}
