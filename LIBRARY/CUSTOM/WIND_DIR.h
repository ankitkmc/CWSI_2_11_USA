/**
 * @file WIND_SPEED.h
 * @brief for class definition of WIND_DIR module used in AWS 2.0 system.
 *  Created on: May 15, 2023
 *  dependencies: ANALOG.h
 *      Author: dhanveer.singh
 */

#ifndef WIND_DIR_H_
#define WIND_DIR_H_

#include <string>
/**
 * for definition of WIND_DIR class used to define WIND_DIR module used in AWS 2.0.
 */
class WIND_DIR: public ANALOG {
		/**
		 * @struct Direction
		 * @brief A structure representing direction fields including - degree,volt,name,max-min bound.
		 */
		struct Direction {
				uint8_t degree;  //!< A 8 bit integer designating degree of rotation.
				double volt = 0;
				string name = "";
				double max = -1, min = -1;  //!< A double value designating min and max bound
		};

		DynamicArray<Direction> direction;  //!< A Dynamic Array object with Direction structure type ,designating possible directions , degree of rotation etc. (described in direction structure).

	public:

		WIND_DIR(DynamicArray<VARIABLES*> *variables_pointer, string var_name, bool add_to_json = 0) :
				ANALOG(variables_pointer, var_name, add_to_json) {
			DIR_SETUP();
		}
		/**
		 * @fn 	string DIRECTION_READ(uint32_t count = 1, bool edit = 1)
		 * @brief To get analog values coming from WIND_DIR and calculate average voltage value and also store in parameter dynamic array of this WIND_DIR object.
		 * @details calculate average analog value and then add assign name string to direction structure for each index of direction dynamic array object of this WIND_DIR object.
		 * @param [in] count  a 32 bit integer value designating count value(no of times) upto which average of analog values coming from sensor is to be taken.
		 * @param [in] edit 1 bit boolean value designating if we have to set calculated analog value to parameter structure of parameter Dynamic array object of this ANALOG class or not.
		 *
		 */
		string DIRECTION_READ(uint32_t count = 1, bool edit = 1) {
			if (GET_VAR_VALUE_CONN() == 1) {
				refresh_counter();
				double in_volt = ANALOG_CONVERT_AVERAGE(count, 0);
				uint8_t index = 0;
				for (; index < direction.size(); index++) {
					if (IN_RANGE(in_volt, index)) {
						if (edit) {
							SET_VALUE_STRING(0, direction.at(index)->name);
							both_debug.Print2("\n\r" + GET_VAR_NAME() + " : " + direction.at(index)->name + "\n\r");
						}
						return direction.at(index)->name;
					}
				}
				if (edit) {
					SET_VALUE_STRING(0, "NO_DIR");
				}
				return "NO_DIR";
			}
			return "SENSOR_OFF";
		}
		/**
		 * @fn 	SENSOR_RETURN SET_VALUE_STRING(uint16_tindex, string strng)
		 * @brief To set string value to a desired index no of parameter dynamic array object of this WIND_DIR object.
		 * @param [in] index index no
		 * @param [in] strng a string value
		 *
		 */
		SENSOR_RETURN SET_VALUE_STRING(uint16_t index, string strng) {
			if (parameter.size() != 0) {
				// if value not present
				if (strng == "") {
					// check if index out of bounds
					if (SET_PARA_VALUE(index, -1) != $OK) {
						return $PARA_SIZE_OUT;
					}
					return $SENSOR_ERROR;
				} else {
					// check if index out of bounds
					if (SET_PARA_VALUE(index, strng) != $OK) {
						return $PARA_SIZE_OUT;
					}
					return $OK;
				}
			} else {
				return $PARA_SIZE_0;
			}
		}
		/**
		 * @fn 	bool IN_RANGE(double in_volt, uint32_t index)
		 * @brief to check if input voltage is in range as per input index number of direction dynamic array object.
		 */
		bool IN_RANGE(double in_volt, uint32_t index) {
			if (in_volt < direction.at(index)->max && in_volt > direction.at(index)->min) {
				return 1;
			} else
				return 0;
		}
		/**
		 * @fn 	void DIR_SETUP()
		 * @brief To setup dynamic array direction object with Direction structure described manually along with sorting of voltages is to be done.
		 */
		void DIR_SETUP() {
			ADD_DIR(0, "N", 2.5344);
			ADD_DIR(22.5, "NNE", 1.3068);
			ADD_DIR(45, "NE", 1.485);
			ADD_DIR(67.5, "ENE", 0.2706);
			ADD_DIR(90, "E", 0.297);
			ADD_DIR(112.5, "ESE", 0.2112);
			ADD_DIR(135, "SE", 0.594);
			ADD_DIR(157.5, "SSE", 0.4092);
			ADD_DIR(180, "S", 0.924);
			ADD_DIR(202.5, "SSW", 0.7854);
			ADD_DIR(225, "SW", 2.0328);
			ADD_DIR(247.5, "WSW", 1.9338);
			ADD_DIR(270, "W", 3.0492);
			ADD_DIR(292.5, "WNW", 2.83);
			ADD_DIR(315, "NW", 3.1548);
			ADD_DIR(337.5, "NNW", 2.2638);
			SORT_VOLTAGES();
			SET_RANGES();
		}
		/**
		 *@fn void ADD_DIR(uint32_t degree, string name, double volt)
		 *@brief TO add direction structure to direction dynamic array object of this WIND_DIR object.
		 *@param [in] degree a 32 bit integer designating degree of rotation of wind direction object.
		 *@param [in] name a string  value designating direction name.
		 *@param [in] volt a double value designating voltage level indicating a particular direction.
		 */
		void ADD_DIR(uint32_t degree, string name, double volt) {
			Direction temp;
			temp.degree = degree;
			temp.name = name;
			temp.volt = volt;
			direction.push_back(temp);
		}
		/**
		 * @fn void SORT_VOLTAGES()
		 * @brief To sort voltages corresponding to direction structure of direction dynamic array object of this WIND_DIR object.
		 */
		void SORT_VOLTAGES() {
			for (uint32_t i_1 = 0; i_1 < direction.size() - 1; i_1++) {
				for (uint32_t i_2 = 0; i_2 < direction.size() - 1 - i_1; ++i_2) {
					if (direction.at(i_2)->volt > direction.at(i_2 + 1)->volt) {
						direction.swap(i_2, i_2 + 1);
					}
				}
			}
		}
		/**
		 * @fn void SET_RANGES()
		 * @brief To set ranges of voltages with max and min bounds corresponding to direction structure of direction dynamic array object of this WIND_DIR object.
		 */
		void SET_RANGES() {
			for (uint32_t iterator = 0; iterator < direction.size(); iterator++) {
				direction.at(iterator)->max = (direction.at(iterator)->volt + (iterator == direction.size() - 1 ? 3.3 : direction.at(iterator + 1)->volt)) / 2;
				direction.at(iterator)->min = ((iterator == 0 ? 0.0 : direction.at(iterator - 1)->volt) + direction.at(iterator)->volt) / 2;
			}
		}

};

#endif /* WIND_SPEED_H_ */
