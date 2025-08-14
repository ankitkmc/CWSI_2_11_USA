                                                      /* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/

#include <main.h>
//#include "dma.h"
#include "usart.h"
#include "iwdg.h"
#include "adc.h"
#include "gpio.h"
#include "rtc.h"


//#define UB1_ON
//#define UL1_ON

//#define neo_cont_print
//#define neo_print_resp_size
#define PRINT_WAKEUP_TIME
//#define ALWAYS_PRINT
#define BLE_ON
//#define BLE_PRINT
// change clock configuration
//#define OTA_ASK

#include "INCLUDER.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
//#define APP_OTA
#if defined(APP_OTA)
const uint32_t appadd = 0x8000000;
#else
const uint32_t appadd = 0x801F000;
#endif
//uint32_t appadd = 0x0800EA60;
typedef void (*ptrFapp)();

void go2app(uint32_t APP_ADDR) {
	uint32_t JumpAddress;
	ptrFapp Jump_app;

	both_debug.Print2("\r\nAPP ADDRESS = " + d_t_h_s(APP_ADDR));
	if (((*(uint32_t*) APP_ADDR) & 0x2FFE0000) == 0x20000000) {
		JumpAddress = *(uint32_t*) (APP_ADDR + 4);
		both_debug.Print2("\tGoing to APP : " + d_t_h_s(JumpAddress));
		Jump_app = (ptrFapp) JumpAddress;
//		uint32_t MSP_ADD = *(uint32_t*) APP_ADDR;
//		both_debug.Print2("\tChanging MSP to: " + d_t_h_s(MSP_ADD));
//		__set_MSP(MSP_ADD);
		Jump_app();
	} else {
		both_debug.Print2("\r\nNo APP : Reseting system");
		NVIC_SystemReset();
	}
}

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
bool check_ota = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// macro to define objects for classes to automatically add variable name for rs485 class
#define create_rs485(class_name, obj_name) \
		class_name obj_name( &huart2, vp, #obj_name, 1)
#define add_to_rs485( obj_name) \
		Rs485_ptr[rs485_ptr_itr++] = &obj_name

//		class_name obj_name( &huart3, vp, #obj_name, 1)

#define create(class_name, obj_name) \
		class_name obj_name(vp, #obj_name, 1)
#define create_0(class_name, obj_name) \
		class_name obj_name(vp, #obj_name, 0)

create(VARIABLES, WS);
create(VARIABLES, DEVICE_ID);
//create_0(VARIABLES, ALARM_TIME);
create_0(VARIABLES, WAKEUP_INT);
create_0(VARIABLES, CHECK_OTA);
create_0(VARIABLES, PRINT);
create_0(VARIABLES, PRINT_ESSENTIALS);
create_0(VARIABLES, INSTALL_CERT);
create_0(BUTTON_READ, BUTTON);
create_0(VARIABLES, EXIT);

JSON_HANDLER data_packet(variables_pointer);
PWR_PIN V_12(EN_12V_GPIO_Port, EN_12V_Pin);

//RTC_DateTypeDef now_date;
//RTC_TimeTypeDef now_time;

#include "FUNCTIONS.h"

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
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
	MX_USART2_UART_Init();
	MX_ADC1_Init();
	MX_LPUART1_UART_Init();
	MX_USART1_UART_Init();
	MX_RTC_Init();
	MX_IWDG_Init();
	/* USER CODE BEGIN 2 */
	if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 5, RTC_WAKEUPCLOCK_CK_SPRE_16BITS) != HAL_OK) {
		Error_Handler();
	}
	object_setup();

#if defined(UB1_ON)
	BUTTON.SET_PIN(UB1_GPIO_Port, UB1_Pin);
#endif
	/* USER CODE END 2 */

//	add_to_rs485(NPK_SENSOR);
//	add_to_rs485(LEAF_SENSOR);
//	add_to_rs485(SOIL_SENSOR);
//	add_to_rs485(AIR_TPH_SENSOR);
//	add_to_rs485(RAIN_GAUGE_SENSOR);
//	add_to_rs485(GEMHO_SOIL_NPK);
//	add_to_rs485(GEMHO_SOIL_TH);
//	add_to_rs485(GEMHO_4_1);
//	add_to_rs485(GEMHO_7_1);
//	add_to_rs485(GEMHO_AIR_TPH);
//	add_to_rs485(CHANGE_ADD);
//	add_to_rs485(CHANGE_ADD2);
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */
		/* USER CODE BEGIN 3 */

#if defined(UL1_ON)
		LED_1.SET(1, 500);
#endif

		ALL_POWER_ON();
//		HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
		neoway.POWER_ON();

		config_file();

#if defined(BLE_ON)
		neoway.BLE_SETUP(d_t_s(WS.GET_VAR_VALUE_CONN()));
#endif

		if (PRINT.GET_VAR_VALUE_CONN() == 1) {
			set_both_print(1);
			both_debug.Print2("\r\nPRINT ON\r\n");
			config_file();
		}

		neoway.INIT();
		Get_save_time();

		PassAuthen();

		both_debug.Print2("\r\nIn OTA CODE  " __TIME__ " - " __DATE__ "\r\n");

		MENU_PRINT(is_any_print());

//		{
//			uint32_t alarmtim_temp = ALARM_TIME.GET_VAR_VALUE_CONN();
//			RTC_TimeTypeDef alarmtime = { alarmtim_temp / 100, alarmtim_temp % 100 };
//			Set_RTC_ALARM(alarmtime, RTC_ALARM_A);
//		}

		if (WS.GET_VAR_VALUE_CONN() == -1) {
			NVIC_SystemReset();
		}

//#define TEST

//		STOPPER("Enter \"1\" to go to OTA_FUCNTION", "1");
		check_all_certificates();
		OTA_FUNCTION();
//		STOPPER("Enter \"1\" to go to application", "1");
//		GO_TO_SLEEP();
		go2app(appadd);
//		STOPPER("Enter \"1\" to Exit", "1");
		NVIC_SystemReset();

	}
	/* USER CODE END 3 */
}

/*
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
		Error_Handler();
	}

	/** Configure LSE Drive Capability
	 */
	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
     ex: printf("Wrong parameters value: file %s on line %d\r\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
