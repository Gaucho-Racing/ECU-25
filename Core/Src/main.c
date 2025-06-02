/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "fdcan.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stateMachine.h"
#include "pinging.h"
#include "msgIDs.h"
#include "customIDs.h"
#include "utils.h"
#include "driving.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
PUTCHAR_PROTOTYPE
{
  ITM_SendChar(ch);
  return ch;
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_FDCAN1_Init();
  MX_FDCAN2_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_LPUART1_UART_Init();
  /* USER CODE BEGIN 2 */


  // 10us ticks
  HAL_SetTickFreq(TICK_FREQ);
  LOGOMATIC("--Boot Finished at Tick %lu--\n", HAL_GetTick());
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t driveActive = 1;
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    //drive_active_power();

    //LOGOMATIC("APPS1 is %d at %f\tAPPS2 is %d at %f\tBSE is %d at %f\n", analogRead(APPS1_SIGNAL), analogRead(APPS1_SIGNAL) * ADC_CONV, analogRead(APPS2_SIGNAL), analogRead(APPS2_SIGNAL) * ADC_CONV, analogRead(BSE_SIGNAL), analogRead(BSE_SIGNAL) * ADC_CONV);

    //LOGOMATIC("APPS 1, APPS 2\n");
    // float brakeTravel = getBrakeTravel();
    // float pedalTravel = getPedalTravel();
    // float throttle1 = getThrottle1();
    // float throttle2 = getThrottle2();
    // bool appsViolation = checkBSEAPPSviolation(throttle1, throttle2, pedalTravel, brakeTravel);
    // float x = (throttle2 - throttle1 * 1.9932988878 - 0.125125991408) / throttle2;
    // x *= 100;
    // pedalTravel *= 100;
    // pedalTravel = pedalTravel < 5 ? 0 : pedalTravel - 5;
    // pedalTravel /= 0.95;
    // LOGOMATIC("%lf, %lf, %lf, %lf\n", pedalTravel, throttle1, throttle2, x);
    // LOGOMATIC(appsViolation ? "APPS VIOLATION\n" : "");

    //drive_active_power();

    float brakeTravel = getBrakeTravel();
    float pedalTravel = getPedalTravel();

    if (checkBSEAPPSviolation(getThrottle1(), getThrottle2(), pedalTravel, brakeTravel))
    {
        globalStatus.ECUState = DRIVE_STANDBY;
        BSE_APPS_violation = true;
        sendBseAppsViolationMessage();
    }

    uint16_t maxCurrent = (uint16_t)MAX_CURRENT_TESTING_AT_OWEN_AND_VAMSI_HOUSE * 10;
    writeDtiMessage(MSG_DTI_CONTROL_8, (uint8_t*)&maxCurrent, 2);

    pedalTravel = pedalTravel < 0.05 ? 0 : (pedalTravel - 0.05) / 0.95;
    uint16_t rearThrottleRequest = (uint16_t)(pedalTravel * 100) << 8;
    writeDtiMessage(MSG_DTI_CONTROL_12, (uint8_t*)&driveActive, 1);
    writeDtiMessage(MSG_DTI_CONTROL_5, (uint8_t*)&rearThrottleRequest, 2);

    float x = (getThrottle2() - getThrottle1() * 1.9932988878 - 0.125125991408) / getThrottle2();
    x *= 100;
    LOGOMATIC("%d\n", rearThrottleRequest);

    // Already exists in stateMachine but copied over for cool factor
    if (globalStatus.ECUState != ERRORSTATE)
    {
        HAL_GPIO_WritePin(TSSI_G_CONTROL_GPIO_Port, TSSI_G_CONTROL_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(TSSI_R_CONTROL_GPIO_Port, TSSI_R_CONTROL_Pin, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(TSSI_G_CONTROL_GPIO_Port, TSSI_G_CONTROL_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(TSSI_R_CONTROL_GPIO_Port, TSSI_R_CONTROL_Pin, GPIO_PIN_SET);  // TODO Implement 2-5 Hz 50% flash
    }
    
    //while(appsViolation){
    //  ;
    //}
    //LOGOMATIC("%d\n", analogRead(BSE_SIGNAL));
    HAL_Delay(150);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  
  /* USER CODE BEGIN Error_Handler_Debug */

  HAL_GPIO_WritePin(TSSI_G_CONTROL_GPIO_Port, TSSI_G_CONTROL_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TSSI_R_CONTROL_GPIO_Port, TSSI_R_CONTROL_Pin, GPIO_PIN_SET);  // TODO Implement 2-5 Hz 50% flash

  __disable_irq();

  if (globalStatus.TractiveSystemVoltage >= TS_VOLTAGE_OFF_LIMIT)
  {
    globalStatus.ECUState = TS_DISCHARGE_OFF;
  }
  else
  {
    globalStatus.ECUState = ERRORSTATE;
  }

  HAL_FDCAN_DeInit(&hfdcan1);
  HAL_FDCAN_DeInit(&hfdcan2);
  LOGOMATIC("\n--Error Handler Called--\nEverything Is Broken\n");

  HAL_Delay(3);

  HAL_FDCAN_Init(&hfdcan1);
  HAL_FDCAN_Init(&hfdcan2);

  __enable_irq();

  // Tick so we can power down
  stateMachineTick();
  stateMachineTick(); // Just in case

  while(true)
  {
    LOGOMATIC("---ECU got cooked---\n");
    writeMessage(PrimaryBusCAN, MSG_DEBUG_2_0, GR_ALL, (uint8_t*)"ECU Fail", 8);
    HAL_Delay(250);
    writeMessage(DataBusCAN, MSG_DEBUG_FD, GR_ALL, (uint8_t*)"ECU Internal Failure", 20);
    HAL_Delay(250);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
