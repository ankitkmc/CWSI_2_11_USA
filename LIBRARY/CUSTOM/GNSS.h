/*
 * GNSS.h
 *
 *  Created on: 16-Aug-2023
 *      Author: dhanveer.singh
 */

#ifndef GNSS_H_
#define GNSS_H_

/**
 * for definition of NEOWAY class used to define NEOWAY module used in AWS 2.0.
 */
class GNSS {

	UART_HandleTypeDef *GNSSUART;  //!<Address of a UART object being used to communicate with NEOWAY module connected externally to NEOWAY module.
	struct GPIO_PIN {
		GPIO_TypeDef *GPIOx;
		uint16_t GPIO_PINx;
	};  //!<a structure that is used to configure GPIO pins for UART communication.
	string data_pub_topic = "";  //!<a string that stores topic of data to be published.
	string meta_pub_topic = "";  //!<a string that stores topic of meta_data to be published.
	GPIO_PIN pwr_pin;  //!<a structure variable instance of type GPIO_PIN.
	string response = "";  //!<a string that stores response coming from UART communication by NEOWAY module for data+meta data publishing.
	NEOWAY_RETURN return_check;  //!<an array of size 10 of type NEOWAY_RETURN that stores NEOWAY responses.
public:

	/**
	 * a constructor for NEOWAY class object with input parameters
	 * @param [in] NEOUART A pointer pointing to UART_HanfleTypeDef object.
	 */
	GNSS(UART_HandleTypeDef *GNSSUART) {
		this->GNSSUART = GNSSUART;
	}


	/**
	 * @fn string SEND_RECIEVE(string command, initializer_list<uint32_t> timeout = { 0 }, uint16_t try_count = 1, initializer_list<string> ex_resp = { "OK", "", "" })
	 * @brief To send or receive commands to AWS from NEOWAY module.
	 * @details Iterating through each no of trials following instructions are to be executed.
	 *
	 *    -HAL_UART_Transmit of input command to NEOWAY through NEOART Object.
	 *
	 *    -Now we go on iterating until timeout.size() is achieved.
	 *
	 *    -under each iteration HAL_UART_Receive is done to receive the response and then response is checked and accordingly stored in return check array .Once we are did all iterations return check is checked if expected response is achieved or not. if yes then break that complete trial loop and return the final response otherwise next trial is done to retry sending command to neoway module until unless total no of possible input trials are finished.
	 * @param [in] command It accepts AT commands that are to be send to NEOWAY Module to communicate with and setup it.
	 * @param [in] timeout initially initialized to 0.
	 * @param [in] try_count Designates no of times send/receive process have to be initiated if not done last time correctly .Initialized to 1.
	 * @param [in]  ex_resp Initialized as list of 3 items: "OK"," "," "
	 * @return A string designating NEOWAY response.
	 */
	string GET_LAT_LONG(initializer_list<uint32_t> timeout = { 0 }, uint16_t try_count = 1, initializer_list<string> ex_resp = { "OK", "", "" }) {
		string accu_response = "";  // accumulated response
		if (gnss_cont == $CONTINUE) {
//			command.push_back(0x0d);
			while (try_count--) {
//			both_debug.Print2("\n\r-->\n\r" + command + "\n\r----------\n\r");
//				HAL_UART_Transmit(GNSSUART, (uint8_t*) command.c_str(), command.size(), UINT32_MAX);
				uint8_t i = 0;
				for (; i < timeout.size(); i++) {
					response = "";
					HAL_UART_Init(GNSSUART);
					//					HAL_UARTEx_ReceiveToTill(GNSSUART, &response, *(timeout.begin() + i), break_string);
					HAL_UARTEx_ReceiveToIdle(GNSSUART, response, *(timeout.begin() + i));
					return_check = CHECK_RESPONSE(&response, i < ex_resp.size() ? (string*) (ex_resp.begin() + i) : NULL);
					accu_response = accu_response + response;
				both_debug.Print2(response);
					if (return_check == $EXPECTED_RESPONSE) {
					both_debug.Print2("\t{=}");
					} else {
					both_debug.Print2("\t{!}");
					}
				}
			both_debug.Print2("\n\r<--\n\r");
//				delay(100 * us_ms);
//				delay(100 * us_ms);

				if (return_check == $EXPECTED_RESPONSE) {
					gnss_cont = $CONTINUE;
				both_debug.Print2("\n\rgnss_cont = CONTINUE\n\r");
					return accu_response;

				} else if (return_check != $NO_RESPONSE || return_check != $ERROR_RESPONSE) {
					if (try_count > 1) {
						gnss_cont = $REPEAT;
					both_debug.Print2("\n\rgnss_cont = REPEAT\n\r");
					} else {
						gnss_cont = $BREAK;
					both_debug.Print2("\n\rgnss_cont = BREAK\n\r");
					}
				} else {
					gnss_cont = $BREAK;
				both_debug.Print2("\n\rgnss_cont = BREAK\n\r");
					break;
				}
			}
			HAL_UART_DeInit(GNSSUART);
			return accu_response;
		} else {
		both_debug.Print2("\n\rBREAKING\n\r");
		}
	}

	/**
	 * @fn NEOWAY_RETURN CHECK_RESPONSE(string *response, string *ex_resp)
	 * @brief To check incoming response to NEOWAY.
	 * @detail as per ex_response and response being fed to input Possible NEOWAY_RETURN response are being returned.
	 * @param [in] response base address of Response variable which stores string data being received from NEOWAY module through NEOART.
	 * @param [in] ex_resp base address of Ex_Response list which stores expected responses.
	 * @return NEOWAY_RETURN a NEOWAY_RETURN enum designating NEOWAY Return status.
	 */
	NEOWAY_RETURN CHECK_RESPONSE(string *response, string *ex_resp) {
		if (ex_resp == NULL) {
			return $ex_resp_NULL;
		} else if (response->find(ex_resp->c_str()) < response->size()) {
			return $EXPECTED_RESPONSE;
		} else if (response->find("ERROR") > response->size()) {
			return $ERROR_RESPONSE;
		} else if (response->size() == 0) {
			return $NO_RESPONSE;
		} else {
			return $N_ERROR;
		}
		HAL_Delay(1);
	}

	/**
	 * @fn const string& GET_data_pub_topic()
	 * @brief To get string stored in data_pub_topic.
	 * @return data_pub_topic Returns constant string stored in data_pub_topic.
	 */
	const string& GET_data_pub_topic() const {
		return data_pub_topic;
	}

	/**
	 * @fn SET_data_pub_topic(const string &data_pub_topic = "")
	 * @brief To set data_pub_topic of corresponding this NEOWAY object as per input string
	 * @param [in] data_pub_topic a constant string Initially initialized to " ".
	 */
	void SET_data_pub_topic(const string &data_pub_topic = "") {
		this->data_pub_topic = data_pub_topic;
	}

	/**
	 * @fn const string& GET_meta_pub_topic()()
	 * @brief To get string stored in meta_pub_topic.
	 * @return data_pub_topic Returns constant string stored in meta_pub_topic.
	 */
	const string& GET_meta_pub_topic() const {
		return meta_pub_topic;
	}

	/**
	 * @fn SET_meta_pub_topic(const string &meta_pub_topic = "")
	 * @brief To set meta_pub_topic of corresponding this NEOWAY object as per input string
	 * @param [in] meta_pub_topic a constant string Initially initialized to " ".
	 */
	void SET_meta_pub_topic(const string &meta_pub_topic = "") {
		this->meta_pub_topic = meta_pub_topic;
	}
};

#endif /* GNSS_H_ */
