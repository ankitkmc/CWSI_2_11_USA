/**
 *@file IRROMETER.h
 *@brief for class definition of IRROMETER module used in AWS 2.0 system.
 *  Created on: May 11, 2023
 *  	Dependencies: ANALOG
 *      Author: dhanveer.singh
 */

#ifndef IRROMETER_H_
#define IRROMETER_H_
/**
 * for definition of IRROMETER class used to define IRROMETER module used in AWS 2.0.
 */
class IRROMETER: public ANALOG {
		/**
		 * @struct PIN_1_2
		 * @brief for gpio pins configuration
		 */
		struct PIN_1_2 {
				GPIO_TypeDef *PIN1_GPIOx;  //!<An pointer pointing to GPIO_TypeDef object
				uint16_t PIN1_GPIO_PINx;  //!<An integer designating GPIO_PIN no
				GPIO_TypeDef *PIN2_GPIOx;  //!<An pointer pointing to GPIO_TypeDef object
				uint16_t PIN2_GPIO_PINx;  //!<An integer designating GPIO_PIN no

		};
		PIN_1_2 pins;
		double *soil_temperature;  //!< a pointer pointing to a double value designating soil temperature.
		double *soil_connected;  //!< a pointer pointing to a bool value designating if IRROMETER is connected or not.
	public:

		IRROMETER(DynamicArray<VARIABLES*> *variables_pointer, string var_name, bool add_to_json = 0) :
				ANALOG(variables_pointer, var_name, add_to_json) {
		}
		/**
		 * @fn void POLARITY_SELECT(GPIO_PinState pin_1_state, GPIO_PinState pin_2_state)
		 * @brief To select polarity of GPIO pins to be connected to IRROMETER SENSOR.
		 * @param[in] pin_1_state,pin_2_state A GPIO_PinState type object indicating pin states.
		 *
		 */
		void POLARITY_SELECT(GPIO_PinState pin_1_state, GPIO_PinState pin_2_state) {
			HAL_GPIO_WritePin(pins.PIN1_GPIOx, pins.PIN1_GPIO_PINx, pin_1_state);
			HAL_GPIO_WritePin(pins.PIN2_GPIOx, pins.PIN2_GPIO_PINx, pin_2_state);
		}
		/**
		 * @fn 	void SET_PIN1(GPIO_TypeDef *PIN1_GPIOx, uint16_t PIN1_GPIO_PINx) {
		 * @brief To assign GPIO config. to Pin no 1.
		 * @param[in] PIN1_GPIOx A GPIO_TypeDef type object indicating GPIO configuration.
		 * @param[in] PIN1_GPIO_PINx A 16 bit integer indicating pin states.
		 *
		 */
		void SET_PIN1(GPIO_TypeDef *PIN1_GPIOx, uint16_t PIN1_GPIO_PINx) {
			pins.PIN1_GPIOx = PIN1_GPIOx;
			pins.PIN1_GPIO_PINx = PIN1_GPIO_PINx;
		}
		/**
		 * @fn 	void SET_PIN2(GPIO_TypeDef *PIN2_GPIOx, uint16_t PIN2_GPIO_PINx) {
		 * @brief To assign GPIO config. to Pin no 2.
		 * @param[in] PIN2_GPIOx A GPIO_TypeDef type object indicating GPIO configuration.
		 * @param[in] PIN2_GPIO_PINx A 16 bit integer indicating pin states.
		 *
		 */
		void SET_PIN2(GPIO_TypeDef *PIN2_GPIOx, uint16_t PIN2_GPIO_PINx) {
			pins.PIN2_GPIOx = PIN2_GPIOx;
			pins.PIN2_GPIO_PINx = PIN2_GPIO_PINx;
		}
		/**
		 * @fn 	double IRRO_READ(uint32_t count = 1, bool edit = 1)
		 * @brief To get analog values coming from IRROMETER and calculate average Resistance value and also store in parameter dynamic array of this IRROMETER object.
		 * @details at final avg resistance is to be calculated . following instructions are executed in top down manner:
		 *
		 * 			1. calculate SM_vol1 storing average of analog values coming from sensor.
		 * 			2. SM_resistance is calculated as per SM_vol1.
		 * 			3. Again same procedure is repeated for SM_vol2 and SM_resistance is calculated from it.
		 * 			4. Now SM_resistanceavg is to be calculated and conversion to centibars or kilopascal which is then to be stored in dynamic array as per input edit value.
		 *
		 * @param [in] count a 32 bit integer value designating count value(no of times) upto which average of analog values coming from sensor is to be taken.
		 * @return SM_resistanceavg
		 */
		double IRRO_READ(uint32_t count = 1, bool edit = 1) {
			if (GET_VAR_VALUE_CONN() == 1) {
				refresh_counter();

				double SM_vol1, SM_vol2, SM_resistance1, SM_resistance2, SM_resistanceavg;
				const double Rx = 7870, SupplyV = 3.30;
				delay(10 * us_ms);
//			for (int i = 1000; i > 0; i--) {
//				POLARITY_SELECT(GPIO_PIN_SET, GPIO_PIN_RESET);
//				delay(500 * us);
//				POLARITY_SELECT(GPIO_PIN_RESET, GPIO_PIN_SET);
//				delay(500 * us);
//			}
				POLARITY_SELECT(GPIO_PIN_SET, GPIO_PIN_RESET);
				delay(90 * us);
				SM_vol1 = ANALOG_CONVERT(0);
//				SM_vol1 = ANALOG_CONVERT_AVERAGE(count, 0);

				POLARITY_SELECT(GPIO_PIN_RESET, GPIO_PIN_RESET);
				SM_resistance1 = ((Rx * SupplyV) / SM_vol1) - Rx;
				delay(100 * us_ms);
				POLARITY_SELECT(GPIO_PIN_RESET, GPIO_PIN_SET);
				delay(90 * us);
//				SM_vol2 = ANALOG_CONVERT_AVERAGE(count, 0);
				SM_vol2 = ANALOG_CONVERT(0);
				POLARITY_SELECT(GPIO_PIN_SET, GPIO_PIN_SET);

//				both_debug.Print2("\r\nThreshold up - " + d_t_s(threshold_range[0], 5, 1) + " : " + d_t_s(DBL_MIN, 5, 1));
//				both_debug.Print2("\r\nThreshold up - " + d_t_s(threshold_range[1], 5, 1) + " : " + d_t_s(DBL_MAX, 5, 1));
				both_debug.Print2("\r\nSM_vol1 - " + d_t_s(SM_vol1, 5, 1));
				both_debug.Print2("\r\nSM_vol2 - " + d_t_s(SM_vol2, 5, 1));
				both_debug.Print2("\r\nSM_resistance1 - " + d_t_s(SM_resistance1));
				delay(100 * us_ms);

//				if (SupplyV - SM_vol2 < 0) {
//					SM_resistanceavg = IRRO_CONVERT(SM_resistanceavg);
//				} else {
				SM_resistance2 = ((Rx * SM_vol2) / (SupplyV - SM_vol2));
				SM_resistanceavg = (SM_resistance1 + SM_resistance2) / 2;
				both_debug.Print2("\r\nSM_resistance2 - " + d_t_s(SM_resistance2));
//				}
				SM_resistanceavg = IRRO_CONVERT(SM_resistanceavg);

				if (edit) {
					SET_VALUE(0, SM_resistanceavg);
				}
				both_debug.Print2("\r\n" + GET_VAR_NAME() + " : " + d_t_s(SM_resistanceavg, 5, 1) + "\r\n");
				return SM_resistanceavg;
			}
			return 0;
		}
		/**
		 * @fn double IRRO_CONVERT(double wm1_resistance)
		 * @brief To calibrate readings coming from IRROMETER and conversion to Cb.
		 * @details Conversion of Resistance to Centibars.
		 * @param [in] wm1_resistance a double value which is to be calibrated.
		 * short_CB = -4
		 * open_CB = -3
		 */
		double IRRO_CONVERT(double wm1_resistance) {
//			both_debug.Print2("\r\nIRRO_CONVERT_IN - " + d_t_s(wm1_resistance, 5));
			const double open_resistance = 35000, short_resistance = 70.0, short_CB = -4, open_CB = -3, min_res = 550.0;
			double wm1_cb = 0.00, TempC = 24.0;
			if (soil_temperature != NULL && *soil_temperature != 0 && *soil_temperature != -1 && soil_connected != NULL && *soil_connected == 1) {
				TempC = *soil_temperature;
			}
//			both_debug.Print2("\r\nTemperature - " + d_t_s(TempC, 2, 1));
			//*****************CONVERSION OF RESISTANCE TO kPa************************************
			// convert WM1 Reading to Centibars or KiloPascal
			if (wm1_resistance > min_res) {
				if (wm1_resistance > 8000.00) {
					wm1_cb = -2.246 - 5.239 * (wm1_resistance / 1000.00) * (1 + .018 * (TempC - 24.00)) - .06756 * (wm1_resistance / 1000.00) * (wm1_resistance / 1000.00) * ((1.00 + 0.018 * (TempC - 24.00)) * (1.00 + 0.018 * (TempC - 24.00)));
				} else {
					if (wm1_resistance > 1000.00) {
						wm1_cb = (-3.213 * (wm1_resistance / 1000.00) - 4.093) / (1 - 0.009733 * (wm1_resistance / 1000.00) - 0.01205 * (TempC));
					} else {
						wm1_cb = -((wm1_resistance / 1000.00) * 23.156 - 12.736) * (1.00 + 0.018 * (TempC - 24.00));
					}
				}
			} else {
				if (wm1_resistance >= short_resistance) {
					wm1_cb = 0.00;
				}
				if (wm1_resistance < short_resistance) {
					return short_CB;
				}
			}
			if (wm1_resistance >= open_resistance) {
				return open_CB;
			}
			if (-wm1_cb > 0) {
				return -wm1_cb;
			}
			if (wm1_cb > 0) {
				return wm1_cb;
			}
			return -1;

		}
		/**
		 * @fn 	void SET_soil_temperature(double *soil_temperature) {
		 * @brief To update soil_temperature of this IRROMETER Object as per input reference to soil_temperature.
		 * @param [in] soil_temperature A pointer pointing to a double value designating soil temperature value.
		 */
		void SET_soil_temperature(double *soil_temperature) {
			this->soil_temperature = soil_temperature;
		}
		/**
		 * @fn void SET_soil_connected(bool *soil_connected) {
		 * @brief To update soil_connected of this IRROMETER Object as per input reference to soil_connected.
		 * @param [in] soil_temperature A pointer pointing to a bool value designating soil_connected value.
		 */
		void SET_soil_connected(double *soil_connected) {
			this->soil_connected = soil_connected;
		}
};

#endif /* IRROMETER_H_ */
