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
#include "adc.h"
#include "fatfs.h"
#include "i2c.h"
#include "iwdg.h"
#include "usart.h"
#include "rtc.h"
#include "spi.h"
#include "gpio.h"
#include "fatfs_sd.h"

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
// change clock configuration
//#define OTA_ASK

/**
 * Include all header files through single header file
 */
#include "INCLUDER.h"

#include "Sandisk.h"
/**
 * @brief The Address App jumps to after BOOTLOADER
 */

const uint32_t appadd = 0x801F000;
uint8_t sample_count=0;
string filename = "file131.txt"; // new added
//string filename2 = "sample_read.txt"; // new added
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

volatile bool check_ota = 0;
// check git
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
create(VARIABLES, U_TIME);
create_0(VARIABLES, FIRMWARE_VERSION);
create_0(VARIABLES, LON);
create_0(VARIABLES, LAT);
#if defined (SENTEK_LEAF)
float aht_temp,aht_hum;
create(VARIABLES,AIR_HT);
#endif
create_rs485(RS485, NPK_SENSOR);
create_rs485(RS485, LEAF_SENSOR);
create_rs485(RS485, SOIL_SENSOR);

//create_rs485(RS485, RAIN_GAUGE_SENSOR);
//create_rs485(RS485, PRESSURE); hi this is git change
//create_rs485(RS485, GEMHO_SOIL_NPK);
//create_rs485(RS485, GEMHO_SOIL_TH); higit 2 change
//create_rs485(RS485, GEMHO_LEAF);
#if defined (GEMHO_TPH)
create_rs485(RS485, AIR_TPH_SENSOR);
create_rs485(RS485, GEMHO_ILLUMINOSITY);
#elif defined(SENTEK_TPH)
create_rs485(RS485, AIR_PRESSURE);
create_rs485(RS485,  AIR_TH_LUX);
#endif
//create_rs485(RS485, GEMHO_AIR_TPH);// Added 3rd change
//create_rs485(RS485, GEMHO_4_1);// final copy to the master

create_rs485(RS485, CHANGE_ADD);

create(ANALOG, BATTERY);
create(ANALOG, SOLAR_PANEL);
create(WIND_DIR, WIND_DIRECTION_SENSOR);
create(WIND_DIR, WIND_DIR1);
create(IRROMETER, IRROMETER_PRIMARY);
create(IRROMETER, IRROMETER_SECONDARY);
create(WIND_SPEED, WIND_SPEED1);
create(RAIN_GAUGE, RAINFALL);
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

SD2 sd_card_2;



#include "FUNCTIONS.h"

//char *SD_data = NULL; // AES

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	HAL_Init();

	SystemClock_Config();

	  MX_GPIO_Init();
	  MX_ADC1_Init();
	  MX_LPUART1_UART_Init();
	  MX_USART1_UART_Init();
	  MX_USART2_UART_Init();
	  MX_RTC_Init();
	  MX_IWDG_Init();
	  MX_I2C1_Init();
	  MX_SPI2_Init();
	  MX_FATFS_Init();


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

		RAINFALL.SET_VAR_VALUE_CONN(1); //539 RAINFALL VALUE -1

#if defined(BLE_ON)
		neoway.BLE_SETUP(d_t_s(WS.GET_VAR_VALUE_CONN()));
#endif

		if (PRINT.GET_VAR_VALUE_CONN() == 1) {
			set_both_print(1);
			both_debug.Print2("\r\nPRINT ON\r\n");
			config_file();
		}

		neoway.INIT();
		if(Network.GPRS_ON)Get_save_time();
		PassAuthen();

		initSDCard();
	/*	if (both_debug.Both_read_check("Enter 0 to add data in sd_card", 15, "0") != $EXPECTED_RESPONSE) {
		for(int i=0;i<60;i++){
		string	data=   "{\"WS\":537,\"DEVICE_ID\":" + to_string(i) +
			        ",\"TIME\":\"15/10/25 07:"+to_string(i)+":"+to_string(i)+"\",\"HT_LEF_HUM\":\"-1.000\",\"LEAF_TEMP\":\"-1.000\","
			        "\"LEAF_HUM\":\"20.369\",\"SOIL_MOISTURE\":\"65.3125\",\"SOIL_TEMPERATURE\":\"26.2000\",\"ATMOS_PRESSURE\":\"950.6478\","
			        "\"ATMOS_HUMIDITY\":\"76.000\",\"ATMOS_TEMPERATURE\":\"26.3561\",\"SOLAR_RADIATION\":\"56035.2310\",\"BATTERY_VOLTAGE\":\"13.4737\","
			        "\"SOLAR_PANNEL_VOLTAGE\":\"20.3694\",\"WIND_DIRECTION\":\"W\",\"IRROMETER_CB_PRIMARY\":\"6.3564\",\"IRROMETER_CB_SECONDARY\":\"4.3561\","
			        "\"WIND_SPEED\":\"5.3645\",\"RAIN_INTENSITY\":\"0\"}";

			    sd_card_2.write4(data, filename);
			}
	}
	*/

		if(Network.GPRS_ON)Get_save_time();
		both_debug.Print2("\r\nNetwork.GPRS_ON:\t"+d_t_s((double)Network.GPRS_ON));

#if defined(APP_CODE)
		both_debug.Print2("\r\nIn APP CODE  " __TIME__ " - " __DATE__ "\r\n");

#else
		both_debug.Print2("\r\nIn OTA CODE  " __TIME__ " - " __DATE__ "\r\n");
#endif

		MENU_PRINT(is_any_print());
//		I2CSensorREDE(&aht_temp,&aht_hum);
#if defined(APP_CODE)
		if (SENSOR_ONLY.GET_VAR_VALUE_CONN() == 1) {
			SENSOR_ONLY_FUNC();
		}
#endif

#if defined(APP_CODE)

	//	SET_WAKEUP_INTERVAL(WAKEUP_INT.GET_VAR_VALUE_CONN());
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

 // 		sample_count=5;// for getting reading in every cycle
		if(Network.GPRS_ON && sample_count==5 &&  Network.isSDcardInsrted==1){
			Network.isDataAvailable=1;
			neoway.SET_data_pub_topic("AWS/EKL/CWMS/" + d_t_s(WS.GET_VAR_VALUE_CONN(), 0));
			uint8_t retry=2;
			DWORD updateline=0;
			neoway.AWS_CON();
			both_debug.Print2("\r\n Network.AWS_PUSH:\t"+d_t_s(Network.AWS_PUSH)+ "\r\n");
			//sd_card_2.offsetPos=sd_card_2.handleOffset(sd_offset, "");
									while(Network.isDataAvailable){

										json_sd = sd_card_2.readJsonLine2k(filename);

								if(Network.isDataAvailable && (sd_card_2.linecount()!=0)){

					CONNECT:		  if(!Network.AWS_PUSH){
											neo_control = $CONTINUE;
											Network.AWS_PUSH=(neoway.SEND_RECIEVE("AT+AWSCONN=120,1,4", { 7000 }, 3, { "OK" }).find("OK") != string::npos);
										}
										ble_cont = $BREAK;
										neoway.SEND_RECIEVE("AT+AWSPUB=0,1,\"" + neoway.GET_data_pub_topic() + "\"," + to_string(json_sd.length()), { 5000 }, 1, { ">" });
										Network.isDataPushed=(neoway.SEND_RECIEVE(json_sd, { 5000, 5000 }, 1, { "OK", "PUB" }).find("+AWSPUB: OK") != string::npos);
										ble_cont = $CONTINUE;
										both_debug.Print2("\r\n Network.isDataPushed:\t"+d_t_s(Network.isDataPushed)+ "\r\n");
//										restore_ble_print();
										}
									   if((!Network.isDataAvailable) && (sd_card_2.linecount()==0)){
										   	sd_card_2.offsetPos=0;
										    sd_card_2.handleOffset(sd_offset,to_string(sd_card_2.offsetPos));
											sd_card_2.deletefile2(filename);

											break;   //Exit after 3 retry
										}
									   if(!Network.isDataPushed){
												Network.AWS_PUSH=0;
													retry--;
												if(!retry){
												    updateline =((DWORD)json_sd.length()-(sd_card_2.linecount()+1));
												    sd_card_2.readJsonLine(filename,updateline);//update the pointer
													Network.isFileCleared=0;
													break;   //Exit after 3 retry
												}

										goto CONNECT;

										}

									}
		}

		Sensortype();
		fetch_reading();


/*
		SD_data = (char*)calloc(1024, sizeof(char));
	    if (!SD_data) {
	        // handle allocation failure
	    	uart_send("\n************Memory allocation failed*****************\n");
	    }
*/
		both_debug.Print2("\r\nSample count: "+d_t_s((double)sample_count));
		sample_count++;
		if(sample_count==6){
		if (both_debug.Both_read_check("Enter 0 to skip sending data", 15, "0") != $EXPECTED_RESPONSE) {
			neoway.AWS_CON();
			neoway_publish("AWS/EKL/CWMS/" + d_t_s(WS.GET_VAR_VALUE_CONN(), 0),	(uint8_t)Network.isFileCleared);
		}
		sample_count=0;
		}

/*	    if (!SD_data) {
	        // handle allocation failure
	    	uart_send("\n************Memory allocation failed*****************\n");
	    }
	    else{
			main_encrypt_and_send(SD_data);
			uart_send(encrypted_data);
			neoway_publish_app("ASSET/CWMS");
	    }
	    free(SD_data);
*/
		if(neo_control != $CONTINUE && sample_count==0 && Network.isSDcardInsrted==1){
			both_debug.Print2("\n  data saved in SD CARD:"+SD_Data+"\n");
			sd_card_2.write4(SD_Data,filename);
			SD_Data.clear();
			neo_control = $CONTINUE;
		}
		SET_WAKEUP_INTERVAL(WAKEUP_INT.GET_VAR_VALUE_CONN());
        if(neo_control == $CONTINUE || !(sd_card_2.isEmpty(filename)))
			GO_TO_SLEEP();

        if(sample_count==0){
        	both_debug.Print2("\n System reset for next reading.");
        	HAL_PWR_EnableBkUpAccess();
        	HAL_RTCEx_BKUPWrite(&hrtc, APP_RESET_FLAG_REG, APP_RESET_MAGIC);
        	NVIC_SystemReset();
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
