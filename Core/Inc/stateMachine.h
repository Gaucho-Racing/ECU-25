#include <stdint.h>

enum State {
    ECU_FLASH,
    GLV_ON,
    TS_PRECHARGE,
    TS_DISCHARGE_OFF,
    PRECHARGING,
    PRECHARGE_COMPLETE,
    DRIVE_STANDBY,
    DRIVE_ACTIVE,
    DRIVE_REGEN,
    ERROR
};

struct InformationToPassToState {
    int replaceMe;
};

void stateMachineTick(State state, InformationToPassToState info);

State ecu_flash();
State glv_on();
State ts_precharge();
State ts_discharge_off();
State precharging();
State precharge_complete();
State drive_standby();
State drive_active();
State drive_regen();