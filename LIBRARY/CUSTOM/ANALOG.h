/**
 * @file ANALOG.h
 * @brief For a generalized class definition to handle analog sensors, which then would be inherited by analog sensors module such as  IRROMETER
 *	dependencies :	VARIABLES.h
 *					DynamicArray.h

 *  Created on: May 8, 2023
 *      Author: dhanveer.singh
 */

#ifndef INC_ANALOG_H_
#define INC_ANALOG_H_

#include <float.h>

#include "INCLUDER.h"

class ANALOG: public VARIABLES {
		uint32_t channel;  //!< a 32 bit integer designating Channel no for ADC module
		double factor = 1;  //!< a double value to amplify analog value read by sensor,if necessary.Initially initialized to 1
		double offset = 0;  //!< a double value to set offset to analog value read by sensor,if necessary.Initially initialized to 0,that is no offset.
		uint32_t raw_offset = 60;
	public:
		double threshold_range[2] = { DBL_MIN, DBL_MAX };  //!< a double value array of size 2 , designating threshold range for analog value sensed by sensor. It is initially initialized to dbl_min and dbl_max value.
		ANALOG(DynamicArray<VARIABLES*> *variables_pointer, string var_name, bool add_to_json = 0) :
				VARIABLES(variables_pointer, var_name, add_to_json) {

		}

		/**
		 * @brief To get analog value after doing adc conversion.
		 * @details following instructions are to be in top-down sequence only.
		 *
		 *         1.start adc
		 *         2.change channel accordingly
		 *         3.start adc conversion
		 *         4.wait for conversion to end
		 *         5.stop adc
		 *         6.set value if edit input is 1.
		 *         7.return corresponding analog value
		 *
		 * @param[in] edit 1 bit boolean value designating if we have to set calculated analog value to parameter structure of parameter Dynamic array object of this ANALOG class or not.
		 * @return analog_value
		 */
		double ANALOG_READ(bool edit = 1) {
			if (GET_VAR_VALUE_CONN() == 1) {
				double analog_value = 0;
//		HAL_ADC_MspInit (&hadc1);
				// start adc
				// change channel accordingly
				ADC1_CHANNEL_INIT(channel);

				// start adc conversion
				HAL_ADC_Start (&hadc1);
				double analog_temp;
				// wait for conversion to end
				if (HAL_ADC_PollForConversion(&hadc1, 1000) == HAL_OK) {
					analog_value = HAL_ADC_GetValue(&hadc1) + raw_offset;
					analog_temp = analog_value;
				} else {
					// timeout
					analog_value = -1;
				}
				// stop adc
				HAL_ADC_Stop(&hadc1);
//		HAL_ADC_MspDeInit(&hadc1);
				if (edit) {
					// set value
					SET_VALUE(0, analog_value);
					both_debug.Print2("\n\r" + GET_VAR_NAME() + " : " + "AV (F:" + d_t_s(factor, 5) + ",O:" + d_t_s(offset, 5) + ",R:" + d_t_s(analog_temp) + ") : " + d_t_s(analog_value, 5) + "\n\r");
				}
				return analog_value;
			}
			return -1;
		}

		/**
		 * @fn double ANALOG_CONVERT(bool edit = 1)
		 * @brief To convert analog value to allowable threshold range.
		 * @details Invoke CONVERT_VALUE() function and set value to parameter dynamic array as per input edit value.
		 * @param [in] edit 1 bit boolean value designating if we have to set calculated analog value to parameter structure of parameter Dynamic array object of this ANALOG class or not.
		 * @return analog_value Converted allowable Analog value
		 */
		double ANALOG_CONVERT(bool edit = 1) {
			if (GET_VAR_VALUE_CONN() == 1) {
				// convert analog value accordingly
				double analog_value = ANALOG_READ(0);
				double analog_temp = analog_value;
				CONVERT_VALUE(analog_value);
				if (edit) {
					// set value
					SET_VALUE(0, analog_value);
					both_debug.Print2("\n\r" + GET_VAR_NAME() + " : " + "AV (con) (F:" + d_t_s(factor, 5) + ",O:" + d_t_s(offset, 5) + ",R:" + d_t_s(analog_temp) + ") : " + d_t_s(analog_value, 5) + "\n\r");
				}
				return analog_value;
			}
		}

		/**
		 * @fn 	double ANALOG_CONVERT_AVERAGE(uint32_t count = 1, bool edit = 1)
		 * @brief To get average of adjusted analog values and set value to parameter Dynamic Array object as per input edit value.
		 * @param [in] count a 32 bit integer value designating count upto which analog values are to be averaged.
		 * @param [in] edit 1 bit boolean value designating if we have to set calculated analog value to parameter structure of parameter Dynamic array object of this ANALOG class or not.
		 * @return average a double value designating average value of analog values
		 */
		double ANALOG_CONVERT_AVERAGE(uint32_t count = 1, bool edit = 1) {
			// convert analog value accordingly
			DynamicArray<double> analog_value;
			double analog_temp;
			for (uint32_t i = 0; i < count; i++) {
				double temp_value = ANALOG_READ(0);
				analog_temp += temp_value;
				CONVERT_VALUE(temp_value);
				analog_value.push_back(temp_value);
			}
			analog_temp = analog_temp / count;
			double average = 0;
			for (uint32_t i = 0; i < analog_value.size(); i++) {
				average += *analog_value.at(i);
			}
			average = average / analog_value.size();

			if (edit) {
				// set value
				SET_VALUE(0, average);
				both_debug.Print2("\n\r" + GET_VAR_NAME() + " : " + "AV (con avg) (F:" + d_t_s(factor, 5) + ",O:" + d_t_s(offset, 5) + ",R:" + d_t_s(analog_temp) + ") : " + d_t_s(average, 5) + "\n\r");
			}
			return average;
		}

		/**
		 * @fn 	SENSOR_RETURN CONVERT_VALUE(double *analog_value)
		 * @brief To convert input Analog value reference with offset and factor adjustment.
		 * @details following instructions are executed
		 *
		 * 		   *Add offset and factor adjustments.
		 * 		   *Check if adjusted analog value is within threshold range or not.
		 * 		   *if yes then update analog read input reference with added adjustments else update input reference with -2 value designating out of bound value.
		 * @param [in] analog_value a pointer to double value whose value is to be converted to allowable value range.
		 * @return SENSOR_RETURN sensor status that telss if analog va;ue is within threshold range or out of bound.
		 */
		double CONVERT_VALUE(double &analog_value) {
			//	if analog value inside range then save else -2
			double analog_value_temp = (analog_value * factor) + offset;
//			both_debug.Print2("\r\n(" + d_t_s(analog_value) + "->" + d_t_s(analog_value_temp, 5) + ")\r\n");
//			if (analog_value_temp > threshold_range[0] && analog_value_temp < threshold_range[1]) {
			analog_value = analog_value_temp;
			return analog_value_temp;
//			} else {
//				analog_value = -2;
//				return -2;
//			}

		}

		/**
		 * @fn 	void SET_THRESHOLD(double min = -1, double max = -1)
		 * @brief To set Threshold range of analog read
		 * @param [in] min a double value designating minimum value to be set in threshold range.
		 * @param [in] max a double value designating maximum value to be set in threshold range.
		 */
		void SET_THRESHOLD(double min = -1, double max = -1) {
			if (min != -1) {
				threshold_range[0] = min;
			}
			if (max != -1) {

				threshold_range[1] = max;
			}
		}

		/**
		 * @fn SENSOR_RETURN SET_VALUE(uint index, double *analog_value)
		 * @brief To store calculated analog_value in parameter dynamic array if, value calculated is viable.
		 * @details To check value calculated and then decide if adjusted analog_value is suitable to be stored in parameter dynamic array or not
		 * @param [in] index index no of parameter dynamic array at which adjusted analog_value is to be stored.
		 * @param [in] analog_value A pointer pointing to double value designating analog_value given by analog sensor.
		 * @return SENSOR_RETURN Sensor_return status value indicating if value is stored or not.
		 */
		SENSOR_RETURN SET_VALUE(uint16_t index, double &analog_value) {

			if (parameter.size() != 0) {
				// if value not present
				if (analog_value < 0) {
					// check if index out of bounds
					if (SET_PARA_VALUE(index, analog_value) != $OK) {
						return $PARA_SIZE_OUT;
					}
					return $SENSOR_ERROR;
				} else {
					// check if index out of bounds
					if (SET_PARA_VALUE(index, analog_value) != $OK) {
						return $PARA_SIZE_OUT;
					}
					return $OK;
				}
			} else {
				return $PARA_SIZE_0;
			}

		}

		/**
		 * @fn 	void SET_channel(uint32_t channel) {
		 * @brief To set_channel in adc
		 * @param [in] channel 32bit number designating channel number
		 */
		void SET_channel(uint32_t channel) {
			this->channel = channel;
		}

		/**
		 * @fn 	void SET_factor(double factor = 1) {
		 * @brief To set factor for analog value
		 * @param [in] factor a double value designating factor for analog value.Initially initialized to 1.
		 */
		void SET_factor(double factor = 1) {
			this->factor = factor;
		}

		/**
		 * @fn 	void SET_offset(uint32_t offset=0) {
		 * @brief To set offset for analog value
		 * @param [in] offset a double value designating offset for analog value.Initially initialized to 0.
		 */
		void SET_offset(double offset = 0) {
			this->offset = offset;
		}

		void SET_raw_offset(uint32_t raw_offset = 60) {
			this->raw_offset = raw_offset;
		}
}
;

#endif /* INC_ANALOG_H_ */
