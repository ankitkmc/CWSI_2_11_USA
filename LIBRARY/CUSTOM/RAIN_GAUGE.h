/**
 * @file RAIN_GAUGE.h
 * @brief for class definition of RAIN_GAUGE module used in AWS 2.0 system.
 *
 * 	dependencies: VARIABLES.h
 *
 *  Created on: May 17, 2023
 *      Author: dhanveer.singh
 */

#ifndef RAIN_GAUGE_H_
#define RAIN_GAUGE_H_
/**
 * for definition of RAIN_GAUGE class used to define RAIN_GAUGE module used in AWS 2.0.
 */

#define RAIN_GAUGE_BKP_VALID_TAG   RTC_BKP_DR1
#define RAIN_GAUGE_VALID_MAGIC     0xDEADBEEF
#define RAIN_GAUGE_BKP_REG RTC_BKP_DR0
#include "stm32l4xx_hal.h"
extern RTC_HandleTypeDef hrtc;

class RAIN_GAUGE: public VARIABLES {
		uint32_t last_time = 0;  //!< an integer designating last_time Rain drop was fallen.
		uint32_t min_gap = 10;  //!< an integer designating minimum time gap between fall.
		uint32_t no_tips = 0;  //!< an integer designating no of tips of water into tube.

		double tip_ml = 0.2794;  //!< an double designating volume of tips in ml.

private:
		void save_no_tips_to_backup() {
			HAL_PWR_EnableBkUpAccess();
			HAL_RTCEx_BKUPWrite(&hrtc, RAIN_GAUGE_BKP_REG, no_tips);
			HAL_RTCEx_BKUPWrite(&hrtc, RAIN_GAUGE_BKP_VALID_TAG, RAIN_GAUGE_VALID_MAGIC);
		}

	void load_no_tips_from_backup() {
		HAL_PWR_EnableBkUpAccess();
		uint32_t tag = HAL_RTCEx_BKUPRead(&hrtc, RAIN_GAUGE_BKP_VALID_TAG);
		if (tag == RAIN_GAUGE_VALID_MAGIC) {
			no_tips = HAL_RTCEx_BKUPRead(&hrtc, RAIN_GAUGE_BKP_REG);
		} else {
			no_tips = 0;  // Invalid or uninitialized — reset to 0
		}
		//HAL_PWR_DisableBkUpAccess();
	}

	public:
		RAIN_GAUGE(DynamicArray<VARIABLES*> *variables_pointer, string var_name, bool add_to_json = 0) :
				VARIABLES(variables_pointer, var_name, add_to_json) {
			load_no_tips_from_backup();
		}
		/**
		 * @fn uint32_t ADD_TIP()
		 * @brief To get interrupt callback whenever interrupt comes from sensor
		 * @details To update last_time and no_tips as per HAL_GetTick().
		 * @return no_tips Returns updates no of tips.
		 */
		uint32_t ADD_TIP() {  // add this function to interrupt callback
			uint32_t now_time = HAL_GetTick();
			if (last_time + min_gap < now_time) {
				last_time = now_time;
				no_tips++;
			}
//			debug.Print("\r\nTips - " + d_t_s(no_tips) + "\r\n");
			save_no_tips_to_backup();
			return no_tips;
		}
		/**
		 * @fn void CLEAR_TIP()
		 * @brief To reset no of tips.
		 */
		void CLEAR_TIP() {
			no_tips = 0;
			save_no_tips_to_backup();
		}
		/**
		 * @fn double GET_READING(bool edit = 1)
		 * @brief To get readings of total water content collected.
		 * @details If rain gauge is connected then reading is calculated and returned.
		 * @param [in] edit A 1 bit bool value to check if reading calculated is needed to be passed through SET_VALUE() or not and then storing collected value in parameter structure of parameter dynamic array corresponding to this RAIN_GAUGE object.By default initialized to 1.
		 * @return reading designating total water content as per no of tips and tip_ml.
		 */
		double GET_READING(bool edit = 1) {

			if (GET_VAR_VALUE_CONN() == 1) {
				refresh_counter();
				load_no_tips_from_backup();
				double reading = no_tips * tip_ml;
				if (edit) {
					SET_VALUE(0, &reading);
				}
				both_debug.Print2("\n\rRAINFALL - " + d_t_s(reading, 2) + "\n\r");
				return reading;
			}
			return 0;
		}
		/**
		 * @fn SENSOR_RETURN SET_VALUE(uint16_t index, double *rain_mm)
		 * @brief To store calculated rain_mm value in parameter dynamic array if, value calculated is viable.
		 * @details To check value calculated and then decide if calulated rain_mm is suitable to be stored in parameter dynamic array or not
		 * @param [in] index index no of parameter dynamic array at which calculated rain_mm is to be stored.
		 * @param [in] rain_mm A pointer pointing to double value designating volume of rain calculated measured in mm.
		 * @return SENSOR_RETURN Sensor_return status value indicating if value is stored or not.
		 */
		SENSOR_RETURN SET_VALUE(uint16_t index, double *rain_mm) {

			if (parameter.size() != 0) {
				// if value not present
				if (*rain_mm < 0) {
					// check if index out of bounds
					if (SET_PARA_VALUE(index, -1) != $OK) {
						return $PARA_SIZE_OUT;
					}
					return $SENSOR_ERROR;
				} else {
					// check if index out of bounds
					if (SET_PARA_VALUE(index, *rain_mm) != $OK) {
						return $PARA_SIZE_OUT;
					}
					return $OK;
				}
			} else {
				return $PARA_SIZE_0;
			}

		}

		void SET_min_gap(uint32_t min_gap = 100) {
			this->min_gap = min_gap;
		}
};

#endif /* RAIN_GAUGE_H_ */
