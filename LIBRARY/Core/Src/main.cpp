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

/**
 * Un-comment for Various different functionalities
 */

//#define UB1_ON
//#define UL1_ON
//#define neo_cont_print
//#define neo_print_resp_size
#define PRINT_WAKEUP_TIME
//#define ALWAYS_PRINT
#define BLE_ON
//#define BLE_PRINT
//#define OTA_ASK

/**
 * Include all header files through single header file
 */
#include "INCLUDER.h"

/**
 * @brief The Address App jumps to after BOOTLOADER
 */
const uint32_t appadd = 0x801F000;

/**
 * @brief Pointer to reset handler
 */

typedef void (*ptrFapp)();

/**
 * @fn
 * @brief Function to jump to application code
 * @details This code is used to go to application code after bootloader
 * @param[in] APP_ADDR
 */
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

bool check_ota = 0;

void SystemClock_Config(void);

// macro to define objects for classes to automatically add variable name for rs485 class
#define create_rs485(class_name, obj_name) \
		class_name obj_name( &huart2, vp, #obj_name, 1)

#define add_to_rs485( obj_name) \
		Rs485_ptr[rs485_ptr_itr++] = &obj_name

#define create(class_name, obj_name) \
		class_name obj_name(vp, #obj_name, 1)

#define create_0(class_name, obj_name) \
		class_name obj_name(vp, #obj_name, 0)

create(VARIABLES, WS);
create(VARIABLES, DEVICE_ID);

#if defined(APP_CODE) or defined(RTK)
create_0(VARIABLES, FIRMWARE_VERSION);
create_0(VARIABLES, LONG);
create_0(VARIABLES, LAT);

create_rs485(RS485, NPK_SENSOR);
create_rs485(RS485, LEAF_SENSOR);
create_rs485(RS485, SOIL_SENSOR);
create_rs485(RS485, AIR_TPH_SENSOR);
create_rs485(RS485, RAIN_GAUGE_SENSOR);
//create_rs485(RS485, GEMHO_SOIL_NPK);
create_rs485(RS485, GEMHO_SOIL_TH);
//create_rs485(RS485, GEMHO_LEAF);
//create_rs485(RS485, GEMHO_ILLUMINOSITY);
//create_rs485(RS485, GEMHO_4_1);
//create_rs485(RS485, GEMHO_7_1);
//create_rs485(RS485, GEMHO_AIR_TPH);
create_rs485(RS485, SENTEK_AIR_TPH);
create_rs485(RS485, CHANGE_ADD);

create(ANALOG, BATTERY);
create(ANALOG, SOLAR_PANEL);
create(WIND_DIR, WIND_DIRECTION_SENSOR);
create(WIND_DIR, MISOL_DIR);
create(IRROMETER, IRROMETER_PRIMARY);
create(IRROMETER, IRROMETER_SECONDARY);
create(WIND_SPEED, MISOL_SPEED);
create(RAIN_GAUGE, MISOL_RAIN);
create_0(VARIABLES, SENSOR_ONLY);
#endif

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

#include "FUNCTIONS.h"

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	HAL_Init();

	SystemClock_Config();

	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_ADC1_Init();
	MX_LPUART1_UART_Init();
	MX_USART1_UART_Init();
	MX_RTC_Init();
	MX_IWDG_Init();

	if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 5, RTC_WAKEUPCLOCK_CK_SPRE_16BITS) != HAL_OK) {
		Error_Handler();
	}
	object_setup();

#if defined(UB1_ON)
	BUTTON.SET_PIN(UB1_GPIO_Port, UB1_Pin);
#endif

	while (1) {

#if defined(UL1_ON)
		LED_1.SET(1, 500);
#endif

		ALL_POWER_ON();
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

#if defined(APP_CODE)
		both_debug.Print2("\r\nIn APP CODE  " __TIME__ " - " __DATE__ "\r\n");

#else
		both_debug.Print2("\r\nIn OTA CODE  " __TIME__ " - " __DATE__ "\r\n");
#endif

		MENU_PRINT(is_any_print());
#if defined(APP_CODE)
		if (SENSOR_ONLY.GET_VAR_VALUE_CONN() == 1) {
			SENSOR_ONLY_FUNC();
		}
#endif

#if defined(APP_CODE)

		SET_WAKEUP_INTERVAL(WAKEUP_INT.GET_VAR_VALUE_CONN());
		if (check_ota == 1 || CHECK_OTA.GET_VAR_VALUE_CONN() == 1) {

			CHECK_OTA.SET_VAR_VALUE_CONN(0);
			save_meta_neoway();
			ALL_POWER_OFF();
			both_debug.Print2("Restarting to check OTA");
			NVIC_SystemReset();

		}
		static uint8_t no_of_restart = 0;
		if (WS.GET_VAR_VALUE_CONN() == -1) {
			if (no_of_restart < 5) {
				both_debug.Print2("Restarting app code");
				ALL_POWER_OFF();
				no_of_restart++;
				continue;
			} else {
				both_debug.Print2("Restarting due to Neoway");
				NVIC_SystemReset();
			}
		} else {
			no_of_restart = 0;
		}
#endif

#if defined(UL1_ON)
		LED_1.SET(0);
#endif

//#define TEST

#if defined(APP_CODE) or defined(TEST)
		if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK) {
			Error_Handler();
		}

		fetch_reading();
		if (both_debug.Both_read_check("Enter 0 to skip sending data", 15, "0") != $EXPECTED_RESPONSE) {
			neoway.AWS_CON();
			neoway_publish("AWS/EKL/CWMS/" + d_t_s(WS.GET_VAR_VALUE_CONN(), 0));
		}

		if (neo_control == $CONTINUE) {
			GO_TO_SLEEP();
		}

#endif

#if defined(OTA_CODE) and !defined(TEST)

//		STOPPER("Enter \"1\" to go to OTA_FUCNTION", "1");
		check_all_certificates();
		OTA_FUNCTION();
//		STOPPER("Enter \"1\" to go to application", "1");
//		GO_TO_SLEEP();
		go2app(appadd);
//		STOPPER("Enter \"1\" to Exit", "1");
		NVIC_SystemReset();

#endif

	}
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
