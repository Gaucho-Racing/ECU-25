#include "CANdler.h"
#include "stateMachine.h"


void handleCANMessage(uint16_t msgID, uint8_t *data, uint32_t length, uint32_t timestamp) {
    switch(msgID) {
        /* update globals in stateMachine */
    }
}
