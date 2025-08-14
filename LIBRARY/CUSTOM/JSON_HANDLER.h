/*
 * @file JSON_HANDLER.h
 * @brief for class definition of JSON module to handle JSON conversion in AWS 2.0 system.
 *
 *  Created on: May 24, 2023
 *      Author: dhanveer.singh
 */

#ifndef INC_JSON_HANDLER_H_
#define INC_JSON_HANDLER_H_
/**
 * for definition of JSON_HANDLER class used to define JSON_HANDLER module used in AWS 2.0.
 */
class JSON_HANDLER {

	public:
		StaticJsonDocument<1200> data_json;  //!< Designates a static JSON packet with 1024 fields in total.
		StaticJsonDocument<1200> meta_data_json;  //!< Designates a static JSON packet with 1024 fields in total.
		string json_string = "";  //!< Designates a variable that stores JSON string
		string meta_json_string = "";  //!< Designates a variable that stores JSON string
		DynamicArray<VARIABLES*> &variables_pointer_2;
		enum clear_enum {
			$CLEAR_JSON,  //!<To clear JSON Packet
			$CLEAR_STRING,  //!<To clear JSON string
			$CLEAR_ALL,  //!< To clear both Packet,string
			$CLEAR_NONE  //!< To clear nothing
		};  //!<an enum that stores status about clearing JSON Packet, or JSON string

		JSON_HANDLER(DynamicArray<VARIABLES*> &variables_pointer_3) :
				variables_pointer_2(variables_pointer_3) {
		}
		/**
		 * @fn 	void MAKE_DATA_JSON(DynamicArray<VARIABLES*> variables_pointer_2, clear_enum clear_enum_temp = $CLEAR_NONE, bool serialize = 0)
		 * @brief To make Data into JSON format.
		 * @details at  first clear_enum_temp is cleared so as to start a new session of making JSON data.
		 *
		 * Now >iterating for each and every VARIABLES* Pointer present in Variable Pointer Dynamic Array Object ->
		 *
		 * >> Is_add_to_json bool variable and  Var_value_conn>0 are checked if both gives true only then >>>parameter size is checked whether positive or not , if +ve
		 *
		 *              we again iterate for each and every parameter present in corresponding variable object , we go on adding String VALUE value to each parameter by passing string parameter name to PACKET.
		 *
		 *                     >>>else only Variable's  Value Conn is added to Packet object by passing VAR_name string as parameter.

		 *     >Now if we have to serialize, SerializeJson function is invoked.
		 * @param [in] variables_pointer_2 base address of Dynamic Array object with VARIABLES* type.
		 * @param [in] clear_enum_temp an enum storing status about json data. Initially initialized to $CLEAR_NONE
		 * @param [in] serialize A 1 bit Boolean value storing status about serialization.Initially initialized to 0
		 */
		void MAKE_DATA_JSON(clear_enum clear_enum_temp = $CLEAR_NONE, bool serialize = 0) {
			CLEAR(clear_enum_temp);
			for (uint32_t iter_1 = 0; iter_1 < variables_pointer_2.size(); iter_1++) {
				VARIABLES *temp_ptr = *(variables_pointer_2.at(iter_1));
				if (temp_ptr->IS_add_to_json() && temp_ptr->GET_VAR_VALUE_CONN() > 0) {
					if (temp_ptr->GET_PARA_SIZE() > 0) {
						for (uint32_t iter_2 = 0; iter_2 < temp_ptr->GET_PARA_SIZE(); iter_2++) {
							if (temp_ptr->GET_NAME(iter_2) != "SKIP") {
								data_json[temp_ptr->GET_NAME(iter_2)] = temp_ptr->GET_VALUE(iter_2);
							}
							HAL_Delay(100);
						}
					} else {
						data_json[temp_ptr->GET_VAR_NAME()] = temp_ptr->GET_VAR_VALUE_CONN();
					}
				}
			}
			if (serialize) {
				serializeJson(data_json, json_string);
			}
		}
		/**
		 * @fn 		void MAKE_META_JSON(DynamicArray<VARIABLES*> variables_pointer_2, clear_enum clear_enum_temp = $CLEAR_NONE, bool serialize = 0)
		 * @brief To make Data into JSON format.
		 * @details at first clear_enum_temp is cleared so as to start a new session of making JSON data
		 *
		 * Now >iterating for each and every VARIABLES* Pointer present in Variable Pointer Dynamic Array Object ->
		 *
		 * >> Is_add_to_json bool variable Is checked if it gives true only then
		 *
		 *            Variable's  Value Conn is added to Packet object by passing VAR_name string as parameter.
		 *
		 * >Now if we have to serialize, SerializeJson function is invoked.
		 * @param [in] variables_pointer_2 base address of Dynamic Array object with VARIABLES* type.
		 * @param [in] clear_enum_temp an enum storing status about json data. Initially initialized to $CLEAR_NONE
		 * @param [in] serialize A 1 bit Boolean value storing status about serialization.Initially initialized to 0
		 */
		void MAKE_META_JSON(clear_enum clear_enum_temp = $CLEAR_NONE, bool serialize = 0) {
			CLEAR_META(clear_enum_temp);
			for (uint32_t iter_1 = 0; iter_1 < variables_pointer_2.size(); iter_1++) {
				VARIABLES *temp_ptr = *(variables_pointer_2.at(iter_1));
				if (temp_ptr->GET_VAR_VALUE_CONN() >= 0) {
					meta_data_json[temp_ptr->GET_VAR_NAME()] = temp_ptr->GET_VAR_VALUE_CONN();
					HAL_Delay(100);
				}
			}
			if (serialize) {
				serializeJson(meta_data_json, meta_json_string);
			}
		}
		string GET_JSON_STRING() {
			return json_string;
		}

		uint32_t GET_JSON_STRING_LEN() {
			return json_string.length();
		}

		string GET_META_JSON_STRING() {
			return meta_json_string;
		}

		uint32_t GET_META_JSON_STRING_LEN() {
			return meta_json_string.length();
		}

//	void MAKE_JSON_STRING(DynamicArray<VARIABLES*> *variables_pointer_1) {
//		MAKE_JSON(*variables_pointer_1);
//		serializeJson(packet, json_string);
//	}
		/**
		 * @fn	void CLEAR(clear_enum clear_enum_temp = $CLEAR_NONE)
		 * @brief To clear JSON packet, JSON string
		 * @detail Now as per input enum different switch cases are invoked with provision of clear all, clear json packet, or clear json string
		 * @param [in] clear_enum_temp It is initialized to $clear_none.
		 */
		void CLEAR(clear_enum clear_enum_temp = $CLEAR_ALL) {
			switch (clear_enum_temp) {
				case $CLEAR_ALL:
					data_json.clear();
					json_string.clear();
					break;
				case $CLEAR_JSON:
					data_json.clear();
					break;
				case $CLEAR_STRING:
					json_string.clear();
					break;
				default:
					break;
			}
		}
		void CLEAR_META(clear_enum clear_enum_temp = $CLEAR_ALL) {
			switch (clear_enum_temp) {
				case $CLEAR_ALL:
					meta_data_json.clear();
					meta_json_string.clear();
					break;
				case $CLEAR_JSON:
					meta_data_json.clear();
					break;
				case $CLEAR_STRING:
					meta_json_string.clear();
					break;
				default:
					break;
			}
		}

		const StaticJsonDocument<1200>& GET_meta_data_json() const {
			return meta_data_json;
		}
		const StaticJsonDocument<1200>& GET_data_json() const {
			return data_json;
		}

		bool COMPARE_DATA_JSON(StaticJsonDocument<1200> &other) const {
			return data_json == other;
		}
		bool COMPARE_META_DATA_JSON(StaticJsonDocument<1200> &other) const {
			return meta_data_json == other;
		}
};

#endif /* INC_JSON_HANDLER_H_ */

