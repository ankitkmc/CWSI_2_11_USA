/*
 * includer.h
 *
 *  Created on: Apr 19, 2023
 *      Author: dhanveer.singh
 */

#ifndef INC_INCLUDER_H_
#define INC_INCLUDER_H_


using namespace std;
#include "main.h"

#include "../CUSTOM/ENUMS.h"

volatile LOOP_CONT neo_control = $CONTINUE;
volatile LOOP_CONT ota_cont = $CONTINUE;
volatile LOOP_CONT gnss_cont = $CONTINUE;
volatile LOOP_CONT watchdog_cont = $CONTINUE;
#if defined(BLE_ON)
volatile LOOP_CONT ble_cont = $CONTINUE;
#else
volatile LOOP_CONT ble_cont = $BREAK;
#endif
#include <string>
#include "../CUSTOM/double_to_string.h"
#include <initializer_list>
#include "../CUSTOM/DYNAMIC_ARRAY.h"
#include "../CUSTOM/M_USART.h"
#include "../ArduinoJson-6.x/ArduinoJson.h"
double analog_value[100];
uint32_t analog_index = 0;
string direction_return = "";

struct Meta {
		double value_double;
		string name = "";
		bool meta_update = 0, data_update = 0;
};

struct Parameter {
		double value_double;
		string value_string;
		string name = "";
};

string config_file_name = "config.json";

#define not_found 0xFFFFFFFF

#define us_s 1000000
#define us_ms 1000
#define us 1
#define ms_s 1000
#define s_hr 3600

void delay(volatile uint32_t del) {
	del = (del / 10) * (HAL_RCC_GetSysClockFreq() / 1000000UL);
	while (del) {
		del--;
	}
}

volatile uint32_t reset_counter = UINT32_MAX;
const uint32_t refresh_value = 300;  // 500 = 1 second

inline void refresh_counter() {
	watchdog_cont = $CONTINUE;
}

#include "../CUSTOM/VARIABLES.h"
DynamicArray<VARIABLES*> variables_pointer;
#define vp &variables_pointer

#include "../CUSTOM/PWR_PIN.h"
#if defined(UL1_ON)
	PWR_PIN LED_1(UL1_GPIO_Port, UL1_Pin);

#endif

#if defined(AWS_2_0)
PWR_PIN MUX0(MUX0_GPIO_Port, MUX0_Pin);
PWR_PIN MUX1(MUX1_GPIO_Port, MUX1_Pin);
PWR_PIN MUX2(MUX2_GPIO_Port, MUX2_Pin);

#include "../CUSTOM/MUX.h"
MUX mux(&MUX0, &MUX1, &MUX2);
#endif

#include "../CUSTOM/PRINT_CONTROL.h"
#include "../CUSTOM/UART_PRINTER.h"
#if defined(AWS_2_0)
UART_PRINTER debug(&hlpuart1, "123");
#else
UART_PRINTER debug(&huart2, "123");
#endif

#include "../CUSTOM/NEOWAY.h"
NEOWAY neoway(&huart1);

#include "../CUSTOM/UART_DEBUG.h"
UART_DEBUG both_debug(vp, &debug, &neoway);

#include "../CUSTOM/BUTTON_READ.h"
#if !defined(MOHIT) and !defined(OTA_CODE)
#include "../CUSTOM/RS485.h"
#include "../CUSTOM/ANALOG.h"
#include "../CUSTOM/IRROMETER.h"
#include "../CUSTOM/WIND_DIR.h"
#include "../CUSTOM/WIND_SPEED.h"
#include "../CUSTOM/RAIN_GAUGE.h"
#include "../CUSTOM/GNSS.h"
#endif

#include "../CUSTOM/JSON_HANDLER.h"

/*	OBJECT DEFINITIONS	*/

#endif /* INC_INCLUDER_H_ */

