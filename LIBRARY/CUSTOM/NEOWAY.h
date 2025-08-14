/**
 *@file NEOWAY.h
 *@brief for class definition of NEOWAY module used in AWS 2.0 system.
 *  Created on: May 18, 2023
 *      Author: dhanveer.singh
 */

#ifndef INC_NEOWAY_H_
#define INC_NEOWAY_H_

/**
 * for definition of NEOWAY class used to define NEOWAY module used in AWS 2.0.
 */
class NEOWAY {

	public:
		UART_HandleTypeDef *NEOUART;  //!<Address of a UART object being used to communicate with NEOWAY module connected externally to NEOWAY module.
		string data_pub_topic = "";  //!<a string that stores topic of data to be published.
		string meta_pub_topic = "";  //!<a string that stores topic of meta_data to be published.
		string /*response = "",*/sub_resp = "", ble_resp = "", dma_resp = "";  //!<a string that stores response coming from UART communication by NEOWAY module for data+meta data publishing.
		volatile bool ble_init = 0, sending_ble = 0;
		volatile NEOWAY_RETURN return_check;  //!<an array of size 10 of type NEOWAY_RETURN that stores NEOWAY responses.
		uint8_t dma_char[100];
		PWR_PIN V_3_8;
		PWR_PIN NW_PWR;
//		(V3_8_GPIO_Port, V3_8_Pin)

		string rootca_name = "rooca";
		string clientcert_name = "cltcert";
		string clientkey_name = "cltkey";

		/**
		 * a constructor for NEOWAY class object with input parameters
		 * @param [in] NEOUART A pointer pointing to UART_HanfleTypeDef object.
		 */
		NEOWAY(UART_HandleTypeDef *NEOUART) :
				V_3_8(V3_8_GPIO_Port, V3_8_Pin), NW_PWR(NEOWAY_PWR_GPIO_Port, NEOWAY_PWR_Pin) {
			this->NEOUART = NEOUART;
#if defined(DMAUART)
			START_DMA();
			PAUSE_DMA();
#endif
		}

		/**
		 * @fn Vvoid RESTART()
		 * @brief GPIO_Pins are rewritten as per pwr_pin structure made by us with different hal delays then during PWR_setup time.
		 */
		void START() {
			V_3_8.PULSE(0, 2 * us_s, 1 * us_s);
			NW_PWR.PULSE(1, 3 * us_s, 2 * us_s);

		}

		void RESTART() {
			V_3_8.PULSE(0, 2 * us_s, 1 * us_s);
			NW_PWR.PULSE(1, 3 * us_s, 2 * us_s);

		}


		string GET_BLE_RESP() {
			string resp_ret = ble_resp;
			ble_resp.clear();
			return resp_ret;
		}

		string GET_SUB_RESP() {
			string resp_ret = sub_resp;
			sub_resp.clear();
			return resp_ret;
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
		string SEND_RECIEVE(string command, initializer_list<uint32_t> timeout = { 0 }, uint16_t try_count = 1, initializer_list<string> ex_resp = { "OK", "", "" }) {

			if (neo_control == $CONTINUE) {
				refresh_counter();
				string accu_response(5000, ' ');  // accumulated response
				string response(5000, ' ');
				string ble_temp;
				accu_response.clear();
//				string response;
//				debug.Print("\r\nstring size : " + d_t_s(response.max_size()) + "\r\n");
				command.push_back(0x0D);
				command.push_back(0x0A);

				while (try_count--) {
					debug.Print("\r\n---->\r\n" + command + "\r\n---------- " + d_t_s(try_count) + "\r\n");
					BLE_SEND("\r\n-->\r\n" + command + "\r\n---------- " + d_t_s(try_count) + "\r\n");
//					HAL_UART_DeInit(NEOUART);
					HAL_UART_Init(NEOUART);
					HAL_UART_Transmit(NEOUART, (uint8_t*) command.c_str(), command.size(), UINT32_MAX - 1);
					uint32_t i = 0;

					for (; i < timeout.size(); i++) {
						response.clear();
						refresh_counter();
						HAL_UARTEx_ReceiveToIdle(NEOUART, response, (uint32_t) *(timeout.begin() + i));
						refresh_counter();
#if defined(neo_print_resp_size)
						debug.Print("< " + d_t_s(response.size()) + " > ");
#endif

						return_check = CHECK_RESPONSE(&response, i < ex_resp.size() ? (string*) (ex_resp.begin() + i) : NULL);

						accu_response = accu_response + response;
					}

					if (return_check == $EXPECTED_RESPONSE) {
						debug.Print("\t{=}");
						BLE_SEND("\t{=}");
					} else {
						BLE_SEND("\t{!}");
						debug.Print("\t{!}");
					}

					debug.Print(accu_response + "\r\n<--\r\n");
					BLE_SEND(accu_response + "\r\n<--\r\n");

					uint32_t rec_loc = accu_response.find("+AWSSUBRECV");
					uint32_t ble_rec_loc = accu_response.find("BLEPRECV");

					if (rec_loc != not_found) {
						uint32_t end_loc = accu_response.find('\n', rec_loc);
						sub_resp = accu_response.substr(rec_loc, end_loc - rec_loc);
						debug.Print("\r\nSUB_RESP : " + sub_resp);
					}

					if (ble_rec_loc != not_found) {
						uint32_t ble_end_loc = accu_response.find('\n', ble_rec_loc);

						ble_temp = accu_response.substr(ble_rec_loc, ble_end_loc);
						uint32_t ble_data_loc = ble_temp.find_last_of(',');
						if (ble_data_loc != not_found) {
							ble_temp = ble_temp.substr(ble_data_loc + 1);
							while (ble_temp.back() == '\n' || ble_temp.back() == '\r') {
								ble_temp.pop_back();
							}
							ble_resp = ble_temp;
						}

					}
					refresh_counter();
					if (return_check == $EXPECTED_RESPONSE || return_check == $ex_resp_NULL) {
						neo_control = $CONTINUE;
#if defined(neo_cont_print)
						debug.Print("\r\nneo_control = CONTINUE");
#endif

						return accu_response;
					} else
						if (return_check != $NO_RESPONSE || return_check != $ERROR_RESPONSE) {
							if (try_count > 1) {
								neo_control = $REPEAT;
#if defined(neo_cont_print)
								debug.Print("\r\nneo_control = REPEAT - NO/ERROR RESPONSE");
								BLE_SEND("\r\nREPEAT - NO/ERROR RESPONSE");
#endif
							} else {
								neo_control = $BREAK;
#if defined(neo_cont_print)
								debug.Print("\r\nneo_control = BREAK1");
								BLE_SEND("\r\nBREAK1");
#endif
							}
						} else {
							neo_control = $BREAK;
#if defined(neo_cont_print)
							debug.Print("\r\nneo_control = BREAK2");
							BLE_SEND("\r\nBREAK2");
#endif
							break;
						}
				}
				return accu_response;
			} else {
				debug.Print("\r\n\t\tSKIPPING : " + command);
				return "";
//				POWER_ON();
			}
		}

		void SEND_RECIEVE_POINTER(string command, initializer_list<uint32_t> timeout = { 0 }, uint16_t try_count = 1, initializer_list<string> ex_resp = { "OK", "", "" }, string &accu_response = (string&) "") {

			if (neo_control == $CONTINUE) {
				refresh_counter();
				string response(5000, ' ');
				accu_response.clear();
//				string response;
//				debug.Print("\r\nstring size : " + d_t_s(response.max_size()) + "\r\n");
				command.push_back(0x0D);
				command.push_back(0x0A);

				while (try_count--) {
					debug.Print("\r\n---->\r\n" + command + "\r\n---------- " + d_t_s(try_count) + "\r\n");
					BLE_SEND("\r\n-->\r\n" + command + "\r\n---------- " + d_t_s(try_count) + "\r\n");
//					HAL_UART_DeInit(NEOUART);
					HAL_UART_Init(NEOUART);
					HAL_UART_Transmit(NEOUART, (uint8_t*) command.c_str(), command.size(), UINT32_MAX - 1);
					uint32_t i = 0;

					for (; i < timeout.size(); i++) {
						response.clear();
						refresh_counter();
						HAL_UARTEx_ReceiveToIdle(NEOUART, response, (uint32_t) *(timeout.begin() + i));
						refresh_counter();
#if defined(neo_print_resp_size)
						debug.Print("< " + d_t_s(response.size()) + " > ");
#endif

						return_check = CHECK_RESPONSE(&response, i < ex_resp.size() ? (string*) (ex_resp.begin() + i) : NULL);

						accu_response = accu_response + response;
					}

					if (return_check == $EXPECTED_RESPONSE) {
						debug.Print("\t{=}");
						BLE_SEND("\t{=}");
					} else {
						BLE_SEND("\t{!}");
						debug.Print("\t{!}");
					}

					debug.Print(accu_response + "\r\n<--\r\n");
					BLE_SEND(accu_response + "\r\n<--\r\n");
					refresh_counter();

					if (return_check == $EXPECTED_RESPONSE || return_check == $ex_resp_NULL) {
						neo_control = $CONTINUE;
#if defined(neo_cont_print)
						debug.Print("\r\nneo_control = CONTINUE");
#endif

					} else
						if (return_check != $NO_RESPONSE || return_check != $ERROR_RESPONSE) {
							if (try_count > 1) {
								neo_control = $REPEAT;
#if defined(neo_cont_print)
								debug.Print("\r\nneo_control = REPEAT - NO/ERROR RESPONSE");
								BLE_SEND("\r\nREPEAT - NO/ERROR RESPONSE");
#endif
							} else {
								neo_control = $BREAK;
#if defined(neo_cont_print)
								debug.Print("\r\nneo_control = BREAK1");
								BLE_SEND("\r\nBREAK1");
#endif
							}
						} else {
							neo_control = $BREAK;
#if defined(neo_cont_print)
							debug.Print("\r\nneo_control = BREAK2");
							BLE_SEND("\r\nBREAK2");
#endif
							break;
						}
				}
			} else {
				debug.Print("\r\n\t\tSKIPPING : " + command);
//				POWER_ON();
			}
		}
		//				POWER_ON();

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
				return $EXPECTED_RESPONSE;
			} else
				if (response->find(ex_resp->c_str()) < not_found || *ex_resp == "") {
					if (response->find(ex_resp->c_str()) < not_found || *ex_resp == "") {
						return $EXPECTED_RESPONSE;
					} else
						if (response->find("ERROR") == not_found) {
							if (response->find("ERROR") == not_found) {
								return $ERROR_RESPONSE;
							} else
								if (response->size() == 0) {
									return $NO_RESPONSE;
								} else {
									return $N_ERROR;
								}
							HAL_Delay(1);
						}
				}
			return $N_ERROR;
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

		void BLE_NAME(string ws_name) {
			string ble_name = "CWS_" + ws_name;
			SEND_RECIEVE("AT+NWBTBLENAME?", { 2000 }, 1, { ble_name });
			if (neo_control != $CONTINUE) {
				neo_control = $CONTINUE;
				SEND_RECIEVE("AT+NWBTBLENAME=\"" + ble_name + "\"", { 2000 }, 1, { "OK" });
			}
		}

		void BLE_SETUP(string ws_name) {
			save_ble_print(1);
			BLE_NAME(ws_name);
			SEND_RECIEVE("AT+NWBTBLEPWR=1", { 1 * ms_s, 2 * ms_s }, 1, { "OK", "" });
			SEND_RECIEVE("AT+NWBLEDISCON?", { 1 * ms_s }, 1, { "NWBLEDISCON:\r\n1" });
			if (neo_control == $CONTINUE) {
				ble_init = 1;
				delay(1 * us_s);
				BLE_SEND("Connected");
			} else {
				neo_control = $CONTINUE;
				SEND_RECIEVE("", { 5 * ms_s }, 1, { "" });
				SEND_RECIEVE("AT+NWBLEDISCON?", { 1 * ms_s }, 1, { "NWBLEDISCON:\r\n1" });
				if (neo_control == $CONTINUE) {
					ble_init = 1;
					delay(1 * us_s);
					BLE_SEND("Connected");
				} else {
					neo_control = $CONTINUE;
					SEND_RECIEVE("AT+NWBTBLEPWR=0", { 5000 }, 2, { "OK" });

				}
			}
			restore_ble_print();
			neo_control = $CONTINUE;
		}

		string BLE_READ(uint32_t timeout = 5000) {
			if (ble_init == 1 && ble_cont == $CONTINUE && is_ble_print()) {
				LOOP_CONT neo_control_temp = neo_control;
				save_both_print(0);
				NEOWAY_RETURN return_check_temp = return_check;
				neo_control = $CONTINUE;

				SEND_RECIEVE("", { timeout }, 1, { "" });

				neo_control = neo_control_temp;
				return_check = return_check_temp;
				restore_both_print();
			}
			return GET_BLE_RESP();
		}

		void BLE_SEND(string send_string) {
			if (ble_init == 1 && sending_ble == 0 && ble_cont == $CONTINUE && is_ble_print()) {
				LOOP_CONT neo_control_temp = neo_control;
				sending_ble = 1;
				save_both_print(0);
				neo_control = $CONTINUE;
				NEOWAY_RETURN return_check_temp = return_check;
				SEND_RECIEVE("AT+NWBLEPSEND=0,0,0,1," + d_t_s(send_string.size(), 0), { 500 }, 1, { ">" });
				SEND_RECIEVE(send_string, { 100, 100 }, 1, { "OK", "NWBLEPSEND" });
				neo_control = neo_control_temp;
				return_check = return_check_temp;
				sending_ble = 0;
				restore_both_print();
			}
		}

		void POWER_ON() {

			SEND_RECIEVE("AT", { 1000 }, 1, { "OK" });
			if (neo_control != $CONTINUE) {
				neo_control = $CONTINUE;
				START();
			}
			SEND_RECIEVE("AT", { 1000 }, 20, { "OK" });
			SEND_RECIEVE("ATE0", { 1000 }, 10, { "OK" });
//			SEND_RECIEVE("AT+IPR=115200", { 2000 }, 10, { "OK" });

		}

		void POWER_OFF() {
			NW_PWR.PULSE(1, 3 * us_s, 3 * us_s);
			V_3_8.SET(0, 1 * us_s);
		}

		void INIT() {

			SEND_RECIEVE("AT+CPIN?", { 5000 }, 5, { "OK" });
			SEND_RECIEVE("AT+CSQ", { 5000 }, 5, { "OK" });
			SEND_RECIEVE("AT+CREG=2", { 5000 }, 5, { "OK" });
			SEND_RECIEVE("AT+CREG?", { 5000 }, 5, { "2," });
			SEND_RECIEVE("AT+CGATT=1", { 5000 }, 5, { "OK" });
			SEND_RECIEVE("AT+CGATT?", { 5000 }, 5, { "+CGATT: 1" });
//	neoway.SEND_RECIEVE("AT$MYSYSINFO", { 5000 }, 1, { "+CGATT: 1" }); 	//	SEND_RECIEVE("AT+NETAPN=\"move.dataxs.mobi\",\"\",\"\"", { 5000 }, 5, { "OK" });

			SEND_RECIEVE("AT+XIIC=1", { 5000 }, 5, { "OK" });
			SEND_RECIEVE("AT+XIIC?", { 5000 }, 5, { "+XIIC:    1" });
			delay(2 * ms_s);
		}

//#if defined(ITUART)
//		void START_IT() {
//			HAL_UART_Receive_IT(NEOUART, dma_char, 100);
//		}

//		void PAUSE_IT() {
//			HAL_UART_RxCpltCallback(NEOUART);
//		}
//
//		void RESUME_IT() {
//			HAL_UART_DMAResume(NEOUART);
//		}
//#endif

		NEOWAY_RETURN GET_Return() const {
			return return_check;
		}

		/**
		 * @brief neoway initialisation only
		 * @retval none
		 */
		void AWS_CON() {

			//	neoway_init();
			//	neoway.SET_data_pub_topic("AWS/EKL/RTK/0");
			//	neoway.SET_data_pub_topic("AWS/EKL/GEMHO/" + d_t_s(WS.GET_VAR_VALUE_CONN(), 0));
			//	string publish_topic = Test.value_1 ? "AWS/EKL/CWMS/TEST/" + WS.GET_VALUE(0) : "AWS/EKL/CWMS/" + WS.GET_VALUE(0);
			//	string metadata_publish_topic = Test.value_1 ? "AWS/EKL/CWMS/TESTMETA/" + WS.GET_VALUE(0) : "AWS/EKL/CWMSMETA/" + WS.GET_VALUE(0);

			SEND_RECIEVE("AT+AWSTLSCFG=authmode,1", { 5000 }, 2, { "OK" });
			SEND_RECIEVE("AT+AWSTLSCFG=rootca," + rootca_name, { 5000 }, 2, { "OK" });
			SEND_RECIEVE("AT+AWSTLSCFG=clientcert," + clientcert_name, { 5000 }, 2, { "OK" });
			SEND_RECIEVE("AT+AWSTLSCFG=clientkey," + clientkey_name, { 5000 }, 2, { "OK" });
			SEND_RECIEVE("AT+AWSCONNPARAM=a1q2r34gpscepz-ats.iot.us-west-2.amazonaws.com:8883,1", { 5000 }, 2, { "OK" });
			SEND_RECIEVE("AT+AWSTLSCFG?", { 5000 }, 2, { "OK" });
			SEND_RECIEVE("AT+AWSAUTHPARAM=AWS_EKL,test,test", { 5000 }, 2, { "OK"});
			SEND_RECIEVE("AT+AWSCONN=60,1,4", { 30000 }, 2, { "OK" });

		}

}
;
#endif /* INC_NEOWAY_H_ */
