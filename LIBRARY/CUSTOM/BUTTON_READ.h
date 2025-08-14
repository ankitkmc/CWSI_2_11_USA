/*
 * BUTTON_READ.h
 *
 *  Created on: Aug 17, 2023
 *      Author: dhanveer.singh
 */

#ifndef BUTTON_READ_H_
#define BUTTON_READ_H_
class BUTTON_READ: public VARIABLES {

	public:

		struct BUTTONPIN {
				GPIO_TypeDef *PWRPIN_GPIOx;
				uint16_t PWRPIN_GPIO_PINx;
		};
		BUTTONPIN buttonpin;
		GPIO_PinState state = GPIO_PIN_RESET;
		bool rest_state = 0;

		BUTTON_READ(DynamicArray<VARIABLES*> *variables_pointer, string var_name, bool add_to_json = 0) :
				VARIABLES(variables_pointer, var_name, add_to_json) {

		}

		void SET_PIN(GPIO_TypeDef *PWRPIN_GPI_Ox, uint16_t PWRPIN_GPIO_PI_Nx, bool rest_state = 0) {
			buttonpin.PWRPIN_GPIO_PINx = PWRPIN_GPIO_PI_Nx;
			buttonpin.PWRPIN_GPIOx = PWRPIN_GPI_Ox;
			this->rest_state = rest_state;
		}

		double READ_STATE_2(bool edit = 0) {
			double state = HAL_GPIO_ReadPin(buttonpin.PWRPIN_GPIOx, buttonpin.PWRPIN_GPIO_PINx);
			if (state == rest_state) {
				state = 2;
			}
			if (edit) {
				// set value
				SET_VALUE(0, &state);
			}
			return state;
		}

		bool READ_STATE(uint32_t del = 1, string comment = "") {
			both_debug.Print2("\n\r" + comment + "\n\rWaiting button input for : " + d_t_s(del, 0));
			while (del--) {
				if (HAL_GPIO_ReadPin(buttonpin.PWRPIN_GPIOx, buttonpin.PWRPIN_GPIO_PINx) != rest_state) {
					while (HAL_GPIO_ReadPin(buttonpin.PWRPIN_GPIOx, buttonpin.PWRPIN_GPIO_PINx) != rest_state) {

					}
					return 1;
				}
				delay(1);
			}
			return 0;

		}

		/**
		 * @fn SENSOR_RETURN SET_VALUE(uint index, double *analog_value)
		 * @brief To store calculated analog_value in parameter dynamic array if, value calculated is viable.
		 * @details To check value calculated and then decide if adjusted analog_value is suitable to be stored in parameter dynamic array or not
		 * @param [in] index index no of parameter dynamic array at which adjusted analog_value is to be stored.
		 * @param [in] analog_value A pointer pointing to double value designating analog_value given by analog sensor.
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
		void wait_button() {
			while (HAL_GPIO_ReadPin(buttonpin.PWRPIN_GPIOx, buttonpin.PWRPIN_GPIO_PINx) == rest_state) {

			}
			while (HAL_GPIO_ReadPin(buttonpin.PWRPIN_GPIOx, buttonpin.PWRPIN_GPIO_PINx) == (!rest_state)) {

			}
		}

};

#endif /* BUTTON_READ_H_ */
