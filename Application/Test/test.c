// Testing LED lights on STM32

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void ledControl();

GPIO_InitTypeDef led;
GPIO_PinState ledState;
int ledArray[] = {15, 14, 13, 12, 11, 10, 9, 8}
uint32_t count;

int main() {

    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();

    

    return 0;
}