#include <stdint.h>
#include <stdbool.h>

#include "utils.h"
#include "inverter.h"
#include "fdcan.h"
#include "msgIDs.h"
#include "stateMachine.h"
#include "customIDs.h"
#include "driving.h"

volatile InverterSettings globalInverterSettings[3] = {0};

volatile DTI_Data globalInverterData = {0};

void sendInverterCommand(void)
{
    writeDtiMessage(MSG_DTI_CONTROL_12, (uint8_t*)&globalInverterSettings[0].driveEnable, 1);
    writeDtiMessage(MSG_DTI_CONTROL_1, (uint8_t*)&globalInverterSettings[0].acCurrent, 2);

    writeMessage(PrimaryBusCAN, MSG_INVERTER_COMMAND, GR_GR_INVERTER_1, (uint8_t*)&globalInverterSettings[1], 7);
    writeMessage(PrimaryBusCAN, MSG_INVERTER_COMMAND, GR_GR_INVERTER_2, (uint8_t*)&globalInverterSettings[2], 7);
}

void controlInverters(bool driveEnable)
{
    globalInverterSettings[0].driveEnable = (uint8_t)driveEnable;
    globalInverterSettings[1].driveEnable = (uint8_t)driveEnable;
    globalInverterSettings[2].driveEnable = (uint8_t)driveEnable;

    sendInverterCommand();
}
