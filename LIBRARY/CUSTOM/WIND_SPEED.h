/**
 * @file WIND_SPEED.h
 * @brief for class definition of WIND_SPEED module used in AWS 2.0 system.
 *
 * 	Dependencies: VARIABLES.h
 *  Created on: May 15, 2023
 *      Author: dhanveer.singh
 */

#ifndef WIND_SPEED_H_
#define WIND_SPEED_H_
/**
 * @def used as a place holder for fan radius ,by default given value of 7 measured in cm.
 */
#define fan_radius 7 // in cm
/**
 * for definition of WIND_SPEED class used to define WIND_SPEED module used in AWS 2.0.
 */
class WIND_SPEED: public VARIABLES {
		/**
		 * @struct GPIO_PIN
		 * @brief for gpio pins configuration
		 */
		struct GPIO_PIN {
				GPIO_TypeDef *GPIOx;  //!<An pointer pointing to GPIO_TypeDef object
				uint16_t GPIO_PINx;  //!<An integer designating GPIO_PIN no

		};
		GPIO_PIN pins;  //!< An object of type GPIO structure to be used for GPIO configuration.

		double circumference = 2 * 3.1415926 * fan_radius / 100000;  //!< A double value indicating circumference to be covered by WIND_SPEED fan.

	public:

		WIND_SPEED(DynamicArray<VARIABLES*> *variables_pointer, string var_name, bool add_to_json = 0) :
				VARIABLES(variables_pointer, var_name, add_to_json) {
		}
		/**
		 * @fn void SET_PIN(GPIO_TypeDef *GPIOx, uint16_t GPIO_PINx)
		 * @brief To set GPIO configuration
		 * @param [in] GPIOx A pointer to an object of type GPIO_TypeDef.
		 * @param [in] GPIO_PINx An integer indicating GPIO Pin no.
		 */
		void SET_PIN(GPIO_TypeDef *GPIOx, uint16_t GPIO_PINx) {
			pins.GPIOx = GPIOx;
			pins.GPIO_PINx = GPIO_PINx;
		}
		/**
		 * @fn double SPEED_READ(uint32_t count = 1, bool edit = 1)
		 * @brief To read wind speed value coming from WIND_SPEED module
		 * @details Count no of rotations as per HAL_GetTick() and then calculate speed and storing it's value into 0 parameter index of parameter dynamic array of this WIND_SPEED object.
		 * @param [in] count Designates no of count unto which rotations are to be
		 * @param [in] edit a 1 bit bool value
		 * @return speed A double value representing wind speed.
		 */
		double SPEED_READ(uint32_t count = 1, bool edit = 1) {
			if (GET_VAR_VALUE_CONN() == 1) {
				uint32_t start_tick = HAL_GetTick();
				uint32_t rotations = 0;
				refresh_counter();
				while (start_tick + count > HAL_GetTick()) {
					if (READ_PIN_EDGE(0)) {
						rotations++;
					}
				}
				double speed = circumference * rotations * ms_s * s_hr / count;
				if (edit) {
					SET_VALUE(0, &speed);
				}
				both_debug.Print2("\n\r" + GET_VAR_NAME() + " : " + d_t_s(speed, 1));
				return speed;
			}
			return 0;
		}
		/**
		 * @fn SENSOR_RETURN SET_VALUE(uint index, double *analog_value)
		 * @brief To store calculated analog_value in parameter dynamic array if, value calculated is viable.
		 * @details To check value calculated and then decide if calculated analog speed value is suitable to be stored in parameter dynamic array or not
		 * @param [in] index index no of parameter dynamic array at which calculated analog speed value is to be stored.
		 * @param [in] analog_value A pointer pointing to double value designating speed analog value calculated measured in km/hr.
		 * @return SENSOR_RETURN Sensor_return status value indicating if value is stored or not.
		 */
		SENSOR_RETURN SET_VALUE(uint16_t index, double *analog_value) {

			if (parameter.size() != 0) {
				// if value not present
				if (*analog_value < 0) {
					// check if index out of bounds
					if (SET_PARA_VALUE(index, -1) != $OK) {
						return $PARA_SIZE_OUT;
					}
					return $SENSOR_ERROR;
				} else {
					// check if index out of bounds
					if (SET_PARA_VALUE(index, *analog_value) != $OK) {
						return $PARA_SIZE_OUT;
					}
					return $OK;
				}
			} else {
				return $PARA_SIZE_0;
			}

		}
		/**
		 * @fn bool READ_PIN_EDGE(bool check_edge)
		 * @brief To check if pin status and last_edge equal or not.
		 * @param [in] check_edge one bit boolean value designating GPIO_Pin status.
		 * @return 1 bit bool value designating if pin status and last_edge equal or not.
		 */

		bool READ_PIN_EDGE(bool check_edge) {
			static bool last_edge = 0;
			bool pin_status = HAL_GPIO_ReadPin(pins.GPIOx, pins.GPIO_PINx);
			if (pin_status != last_edge) {
				if (pin_status == check_edge) {
					last_edge = pin_status;
					return 1;
				} else {
					last_edge = pin_status;
					return 0;
				}
			} else {
				return 0;
			}
		}
};

#endif /* WIND_SPEED_H_ */
