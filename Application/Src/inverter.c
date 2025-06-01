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

volatile int32_t lastInverterPingMillis = BAD_TIME_Negative1;

volatile uint16_t heatCapacity1 = 0;

volatile uint16_t heatCapacity2 = 0;

void sendInverterCommand(void)
{
    if (millis() - lastInverterPingMillis >= INVERTER_MINIMUM_SENDING_TIME_MS) // Must send every 10 ms for power
    {
        lastInverterPingMillis = millis();

        writeDtiMessage(MSG_DTI_CONTROL_12, (uint8_t*)&globalInverterSettings[0].driveEnable, 1);
        writeDtiMessage(MSG_DTI_CONTROL_1, (uint8_t*)&globalInverterSettings[0].acCurrent, 2);

        //Check heat for first forward motor
        float deltaH =  0.01 * (globalInverterSettings[1].driveEnable * globalInverterSettings[1].acCurrent * globalInverterSettings[1].acCurrent - NOMINAL_CURRENT_FORWARD * NOMINAL_CURRENT_FORWARD);
        heatCapacity1 += (heatCapacity1 + deltaH  > 0) ? deltaH : 0;
        if(heatCapacity1 > MAX_AMK_HEAT_CAP * 0.9 && globalInverterSettings[1].acCurrent > MAX_CURRENT_FORWARD * (1 - ((double)heatCapacity1/MAX_AMK_HEAT_CAP - 0.9) / 0.1)){
            globalInverterSettings[1].acCurrent = MAX_CURRENT_FORWARD * (1 - ((double)heatCapacity1/MAX_AMK_HEAT_CAP - 0.9) / 0.1);
        }

        //Check heat for second motor
        deltaH = 0.01 * (globalInverterSettings[2].driveEnable * globalInverterSettings[2].acCurrent * globalInverterSettings[2].acCurrent - NOMINAL_CURRENT_FORWARD * NOMINAL_CURRENT_FORWARD);
        heatCapacity2 += (heatCapacity2 + deltaH  > 0) ? deltaH : 0;
        if(heatCapacity2 > MAX_AMK_HEAT_CAP * 0.9 && globalInverterSettings[2].acCurrent > MAX_CURRENT_FORWARD * (1 - ((double)heatCapacity2/MAX_AMK_HEAT_CAP - 0.9) / 0.1)){
            globalInverterSettings[2].acCurrent = MAX_CURRENT_FORWARD * (1 - ((double)heatCapacity2/MAX_AMK_HEAT_CAP - 0.9) / 0.1);
        }

        //Wait, how to handle one forward motor overheating but the other one being fine? Does the other one also get limited?
        //TODO

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
