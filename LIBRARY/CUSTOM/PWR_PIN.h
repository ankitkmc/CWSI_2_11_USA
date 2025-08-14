/*
 * PWR_PIN.h
 *
 *  Created on: May 31, 2023
 *      Author: dhanveer.singh
 */

#ifndef INC_PWR_PIN_H_
#define INC_PWR_PIN_H_
class PWR_PIN {

	public:
		struct PWRPIN {
				GPIO_TypeDef *PWRPIN_GPIOx;
				uint16_t PWRPIN_GPIO_PINx;
		};

		PWR_PIN(GPIO_TypeDef *PWRPIN_GPI_Ox, uint16_t PWRPIN_GPIO_PI_Nx) {
			SET_PWRPIN(PWRPIN_GPI_Ox, PWRPIN_GPIO_PI_Nx);
		}

		PWRPIN pwrpin;

		void SET_PWRPIN(GPIO_TypeDef *PWRPIN_GPI_Ox, uint16_t PWRPIN_GPIO_PI_Nx) {
			pwrpin.PWRPIN_GPIO_PINx = PWRPIN_GPIO_PI_Nx;
			pwrpin.PWRPIN_GPIOx = PWRPIN_GPI_Ox;
		}

		void Toggle(uint32_t del = 0) {
			HAL_GPIO_TogglePin(pwrpin.PWRPIN_GPIOx, pwrpin.PWRPIN_GPIO_PINx);
			delay(del);
//		HAL_Delay(del);
		}
		void SET(bool state = 0, uint32_t del = 1) {
			HAL_GPIO_WritePin(pwrpin.PWRPIN_GPIOx, pwrpin.PWRPIN_GPIO_PINx, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
			delay(del);
		}
		void PULSE(bool high_or_low, uint32_t del = 0, uint32_t del2 = 0) {
			SET(high_or_low, del);
			SET(!high_or_low, del2);
		}

};

#endif /* INC_PWR_PIN_H_ */
