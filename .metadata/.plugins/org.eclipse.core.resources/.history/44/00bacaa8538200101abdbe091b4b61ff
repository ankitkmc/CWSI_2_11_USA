/**
 * @file RS485.h
 * @brief for class definition of RS485 module used in AWS 2.0 system.
 *
 * dependencies :	VARIABLES.h
 *					DynamicArray.h
 *
 *  Created on: Apr 19, 2023
 *      Author: dhanveer.singh
 */

#ifndef RS485_H_
#define RS485_H_
#include "INCLUDER.h"

/**
 * @def Used as a place holder for uint8 data type here.
 */
#define size_type uint8_t
/**
 * for definition of RS485 class used to define RS485 module used in AWS 2.0.
 */
class RS485: public VARIABLES {
		// usart define for class
		UART_HandleTypeDef *RS485_USART;  //!<A pointer variable which gonna point to USART_HANDLETYPEDEF Object
		// Individual baud rate
		uint32_t manual_baud = 9600;  //!<TO setup manual baud rate for UART communication.
		// Inquiry frame
		DynamicArray<size_type> frame;	//!<a dynamic array object (**designating Inquiry frame**)with pointer pointing to a dynamic array of numbers of type macro size_type.
		// Clear frame
		DynamicArray<size_type> clear_frame;  //!<a dynamic array object (**designating clear frame**)with pointer pointing to a dynamic array of numbers of type macro size_type.
		int rede_no = 0;  //!<an integer defined for GPIO header for RS485,initialized to 0.
		DynamicArray<double> factor;  //!<A Dynamic Array object of type double designating a factor value for each parameter.
		DynamicArray<uint8_t> no_of_bytes;	//!< A dynamic Array object of type uint8 designating no of bytes for each parameter.
		uint16_t mux_pin = 7;

#define READ_FRAME_TIMEOUT 1000
	public:
		RS485(UART_HandleTypeDef *RS485_USART, DynamicArray<VARIABLES*> *variables_pointer, string var_name, bool add_to_json = 0) :
				VARIABLES(variables_pointer, var_name, add_to_json) {
			this->RS485_USART = RS485_USART;
		}

		// rede gpio pins config
		/**
		 * @struct REDE
		 * @brief for rede gpio pins config
		 */
		struct REDE {
				GPIO_TypeDef *RE_GPIOx;
				uint16_t RE_GPIO_PINx;
				GPIO_TypeDef *DE_GPIOx;
				uint16_t DE_GPIO_PINx;
		};
		REDE rede;

		// fetch reading from sensor
		// DynamicArray<size_type>
		/**
		 * @fn void RS485_READ(bool clear_frame_bool = 0, bool edit = 1)
		 * @brief To fetch reading from sensor.
		 * @detail if( VAR_VALUE_CONN ==1) then Initialize UART.Set GPIO pins.Write frame.reset GPIO pins.Read frame and store it in newly generated read_frame dynamic array. Now further check if (edit is done) then convert read frame to parameter values.
		 * @param [in] clear_frame_bool 1 bit boolean value that keeps track if frame has been cleared or not.By default it's value is 0.
		 * @param [in] edit 1 bit Boolean value that checks if edit has been done or not.By default its value is 1edit
		 */
		void RS485_READ(bool clear_frame_bool = 0, bool edit = 1) {
			if (GET_VAR_VALUE_CONN() == 1) {
				refresh_counter();
#if defined(UL1_ON)
			LED_1.Toggle(0);
#endif
				DynamicArray<size_type> read_frame;
				// Initialize UART
				both_debug.Print2("\n\r" + GET_VAR_NAME() + ": " + d_t_s(rede_no, 0) + " : " + d_t_s(manual_baud) + " : ");
				both_debug.Print2(frame);
				uint8_t tries = 4;
				do {
#if defined(AWS_2_0)
					mux.SELECT(mux_pin);
#endif
					HAL_UART_MspInit(RS485_USART);
					UART_Init(RS485_USART, RS485_USART->Instance, manual_baud);
					// rede HIGH, wait 100ms
					WRITE_REDE(GPIO_PIN_SET, 100);
					//write frame to sensor

					WRITE_FRAME(clear_frame_bool ? &clear_frame : &frame);
					//rede LOW, no wait
					WRITE_REDE(GPIO_PIN_RESET);
					// recieve frame from sensor, save it to temporary Dynamic array
					READ_FRAME(&read_frame);
					// rede HIGH to avoid interference from other sensors, can be disabled/commented if not needed
					both_debug.Print2("\t\tREC: ");
					both_debug.Print2(read_frame);
					//COMPARE_CRC(&read_frame);
					WRITE_REDE(GPIO_PIN_SET, 1);

					// de-initialise uart
					HAL_UART_MspDeInit(RS485_USART);
#if defined(AWS_2_0)
					mux.SELECT(7);
#endif
				} while (read_frame.is_empty() && tries-- > 0);

				if (edit) {
					// convert read frame to parameter values
					SET_VALUE(&read_frame);
				}
				//			return read_frame;
#if defined(UL1_ON)
			LED_1.Toggle(500);
#endif
			}

		}

		void CUSTOM_FRAME(initializer_list<size_type> list, uint32_t del = 0) {
			if (GET_VAR_VALUE_CONN() == 1) {

				DynamicArray<size_type> send_frame;
				send_frame.assign(list);
				ADD_CRC(&send_frame);
				both_debug.Print2("\n\r" + GET_VAR_NAME() + ": " + d_t_s(rede_no, 0) + " : " + d_t_s(manual_baud) + " : ");
				both_debug.Print2(send_frame);

#if defined(AWS_2_0)
				mux.SELECT(mux_pin);
#endif

#if defined(UL1_ON)
				LED_1.Toggle(0);
#endif
				DynamicArray<size_type> read_frame;
				uint8_t tries = 4;
				do {
					// Initialize UART
					HAL_UART_MspInit(RS485_USART);
					UART_Init(RS485_USART, RS485_USART->Instance, manual_baud);
					// rede HIGH, wait 100ms
					WRITE_REDE(GPIO_PIN_SET, 100);
					//write frame to sensor

					WRITE_FRAME(&send_frame);
					//rede LOW, no wait
					WRITE_REDE(GPIO_PIN_RESET);
					// recieve frame from sensor, save it to temporary Dynamic array
					READ_FRAME(&read_frame);
					// rede HIGH to avoid interference from other sensors, can be disabled/commented if not needed
					both_debug.Print2("\t\tREC: ");
					both_debug.Print2(read_frame);
					//COMPARE_CRC(&read_frame);
					WRITE_REDE(GPIO_PIN_SET, 1);
					// de-initialise uart
					HAL_UART_MspDeInit(RS485_USART);
				} while (read_frame.is_empty() && tries-- > 0);

#if defined(AWS_2_0)
				mux.SELECT(7);
#endif
//			return read_frame;
#if defined(UL1_ON)
				LED_1.Toggle(del);
#endif

			}
		}
		// transmit frame to sensor
		/**
		 * @fn HAL_StatusTypeDef WRITE_FRAME(DynamicArray<size_type> *frame).
		 * @brief To  transmit frame to sensor.
		 * @details asks UART module to transmit complete inquiry frame dynamic array object to sensor through RS485_UART object.
		 * @param [in] frame a pointer pointing to Dynamic array object which stores inquiry frame.
		 * @return HAL_Status status of UART transmission
		 */
		HAL_StatusTypeDef WRITE_FRAME(DynamicArray< size_type > *frame) {
			return HAL_UART_Transmit(RS485_USART, (uint8_t*) frame->first(), frame->size(), 100);
		}

		// read frame from sensor
		/**
		 * @fn DynamicArray<size_type> READ_FRAME().
		 * @brief To read frame incoming from sensor.
		 * @details asks UART module to recieve to idle stage from frame and store it in a dynamic array object which can be then returned back.
		 * @return DynamicArray<size_type> a dynamic array object representing frame which is being read from sensor through UART.
		 */
		void READ_FRAME(DynamicArray<size_type> *read_frame) {
//		DynamicArray<size_type> temp_frame;
			// recieve till idle, max size == temp_size, define max wait time for initial frame 200
			HAL_UARTEx_ReceiveToIdle(read_frame, READ_FRAME_TIMEOUT);
			double average = 0;
			for (uint32_t iter = 0; iter < read_frame->size(); iter++) {
				average = average + *read_frame->at(iter);
			}
			if (average == 0) {
				read_frame->clean();
				both_debug.Print2("\tframe cleaned");
			}
		}

		// convert received frame to parameter value
		SENSOR_RETURN SET_VALUE(DynamicArray< size_type > *read_frame) {

			if (parameter.size() != 0) {
				// check if recieved frame is empty
				uint32_t skip_front = 0;
				if (read_frame->size() != 0) {
					while (*read_frame->at(skip_front) < 1 && skip_front < read_frame->size()) {
						skip_front++;
					}
				}
				both_debug.Print2("\r\nskip_front = " + d_t_s(skip_front) + ", read_frame_size =" + d_t_s(read_frame->size()));
				if (skip_front == read_frame->size()) {
					for (uint32_t i = 0; i < parameter.size(); i++) {
						if (SET_PARA_VALUE(i, -1) != $OK) {
							both_debug.Print2("\r\nPARA SIZE OUT");
							return $PARA_SIZE_OUT;
						}
					}
					both_debug.Print2("\r\n$SENSOR_ERROR");
					return $SENSOR_ERROR;
				}
				uint32_t on_byte = 3 + skip_front;
				for (uint32_t index = 0; index < parameter.size(); index++) {
					uint64_t value = 0;
					both_debug.Print2("\n\r\tValues: ");

					for (int i_0 = 0; i_0 < *no_of_bytes.at(index); i_0++) {
						uint8_t temp_value = *read_frame->at(on_byte);
						both_debug.Print2(d_t_h_s(temp_value) + " ");
						value = (value << 8) + temp_value;
						on_byte++;

					}
//					both_debug.Print2("\n\r");
					double double_value = value / *factor.at(index);
					if (SET_PARA_VALUE(index, double_value) != $OK) {
						both_debug.Print2("PARA_SIZE_OUT");
						return $PARA_SIZE_OUT;
					}
					both_debug.Print2("\t" + parameter.at(index)->name + ": " + (parameter.at(index)->value_string.size() > 0 ? parameter.at(index)->value_string : "EMPTY") + "\n\r");

				}
				return $OK;

			} else {
				return $PARA_SIZE_0;
			}

		}

		// write gpio rede to stated value, and wait

		/**
		 * @fn void WRITE_REDE(GPIO_PinState value, uint32_t Delay = 0).
		 * @brief To write gpio rede to stated value, and wait.
		 * @param [in] Value GPIO_PinState.
		 * @param [in] Delay Delay to be provided after GPIO_WritePin assignment. By default initialized to 0.
		 */
		void WRITE_REDE(GPIO_PinState value, uint32_t Delay = 0) {
			HAL_GPIO_WritePin(rede.RE_GPIOx, rede.RE_GPIO_PINx, value);
			HAL_GPIO_WritePin(rede.DE_GPIOx, rede.DE_GPIO_PINx, value);
			delay(Delay);
		}
		/**
		 * @fn uint32_t ADD_PARA(string para_name, double factor = 10.0, uint8_t no_of_bytes = 1).
		 * @brief TO add parameter.A functionally overloaded function over ADD_PARA() defined in VARIABLES.
		 * @detail To add a new parameter in parameter's dynamic array object with input para_name also to add new factor and no of bytes (corresponding to newly added parameter) in factor and no_of_bytes dynamic array object respectively.
		 * @param [in] para_name a string designating Parameter's name to be stored in Parameter structure of newly added Parameter in Parameter's Dynamic Array object of RS485 inheried from VARIABLES.
		 * @param [in] factor a double value to be stored as factor value (corresponding to newly added parameter) in factor's Dynamic Array object.It is initially initialized to 10.
		 * @param [in] no_of_bytes a uint8 value to be stored as no_of_bytes value(corresponding to newly added parameter) in no_of_bytes's Dynamic Array object.It is initially initialized to 1. While assignment (input)*2 value is assigned.
		 * @return parameter_size no of parameters
		 */
		uint32_t ADD_PARA(string para_name, double factor = 10.0, uint8_t no_of_bytes = 1) {
			Parameter temp;
			temp.name = para_name;
			parameter.push_back(temp);
			this->no_of_bytes.push_back(no_of_bytes * 2);
			this->factor.push_back(factor);
			return parameter.size();

		}
		/**
		 * @fn void ADD_CRC(DynamicArray<size_type> *temp_frame)
		 * @brief To Add CRC field in input frame
		 * @detail To get crc corresponding to input frame and add crc to and shifted crc to frame
		 * @param [in] temp_frame a pointer pointing to dynamic array object designating a frame
		 */

		void ADD_CRC(DynamicArray<size_type> *temp_frame) {
			uint16_t crc = GET_CRC(temp_frame);
//		both_debug.Print2("\n\r" + GET_VAR_NAME() + ": CALCUATED CRC : ");
//		both_debug.Print2(decimal_to_hex_string(crc));
			temp_frame->push_back(crc & 0x00FF);
			temp_frame->push_back(crc >> 8);
//		both_debug.Print2("\n\r" + GET_VAR_NAME() + ": NEW FRAME : ");
//		both_debug.Print2(frame);
		}
		/**
		 * @fn void COMPARE_CRC(DynamicArray<size_type> *temp_frame)
		 * @brief To check CRC field in input frame
		 * @detail checking crc field and printing if CRC is correct or incorrect.
		 * @param [in] temp_frame a pointer pointing to dynamic array object designating a frame.
		 */

		void COMPARE_CRC(DynamicArray<size_type> *temp_frame) {
			if (!temp_frame->is_empty()) {
				uint16_t crc = GET_CRC(temp_frame, 1);
				DynamicArray<uint8_t> check_frame;
				check_frame.push_back(crc & 0x00FF);
				check_frame.push_back(crc >> 8);
				if (*temp_frame->at(temp_frame->size() - 2) == *check_frame.at(0) && *temp_frame->at(temp_frame->size() - 1) == *check_frame.at(1)) {
					both_debug.Print2("\t\tCRC CORRECT\n\r");
				} else {
					both_debug.Print2("\t\tCRC INCORRECT\n\r");
				}
			}
		}
		/**
		 * @fn uint16_t GET_CRC(DynamicArray<size_type> *temp_frame, bool calculate = 0)
		 * @brief To compute CRC corresponding to input frame.
		 * @param [in] temp_frame a pointer pointing to dynamic array object designating a frame
		 * @param [in] calculate a bool value designating if crc is to be calculated or not.It is initially initialized to 0.
		 * @return crc a 16 bit integer representing crc corresponding to input dynamic array object.
		 */
		uint16_t GET_CRC(DynamicArray<size_type> *temp_frame, bool calculate = 0) {
			uint16_t crc = 0xFFFF;

			for (uint32_t pos = 0; pos < temp_frame->size() - (calculate ? 2 : 0); pos++) {
				crc ^= *temp_frame->at(pos);          // XOR byte into least sig. byte of crc

				for (int i = 8; i != 0; i--) {    // Loop over each bit
					if ((crc & 0x0001) != 0) {      // If the LSB is set
						crc >>= 1;      // Shift right and XOR 0xA001
						crc ^= 0xA001;
					} else
						// Else LSB is not set
						crc >>= 1;      // Just shift right
				}
			}
			// Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)

			return crc;
		}

// set clear frame from list
		/**
		 * @fn void SET_clear_frame(initializer_list<size_type> list)
		 * @brief To set clear_frame dynamic array object as per input list.
		 * @detail Input list is being assigned to clear_frame dynamic array object along with CRC field being added at the end.
		 * @param [in] list a list of <size-type>
		 */
		void SET_clear_frame(initializer_list<size_type> list) {
			clear_frame.assign(list);
			ADD_CRC(&clear_frame);
		}

// set frame from list
		/**
		 * @fn void SET_frame(initializer_list<size_type> list)
		 * @brief To set frame dynamic array object as per input list.
		 * @detail Input list is being assigned to clear_frame dynamic array object along with CRC field being added at the end, simultaneously no_of_bytes dynamic array object is also updated.
		 * @param [in] list a list of <size-type>
		 */
		void SET_frame(initializer_list<size_type> list) {
			frame.assign(list);
			ADD_NO_OF_BYTES();
			ADD_CRC(&frame);
//		both_debug.Print2("\n\r" + GET_VAR_NAME() + ": " + d_t_s(rede_no, 0) + " : ");
//		both_debug.Print2(frame);
		}
		/**
		 * @fn void ADD_NO_OF_BYTES()
		 * @brief To add no of bytes in frame's dynamic array object.
		 * @detail Calculate total no of bytes as per no_of_bytes dynamic array object also add also push_back() with calculated no of bytes in to frame's dynamic array object.
		 */
		void ADD_NO_OF_BYTES() {
			uint16_t temp_no_of_bytes = 0;
			for (uint32_t iterator = 0; iterator < no_of_bytes.size(); iterator++) {
				temp_no_of_bytes += *no_of_bytes.at(iterator);
			}
			temp_no_of_bytes /= 2;
			frame.push_back(temp_no_of_bytes >> 8);
			frame.push_back(temp_no_of_bytes & 0x00FF);
		}
		/**
		 * @fn void SET_REDE(int rede_no = 0)
		 * @brief To set GPIO configuration as per input rede_no.
		 * @param [in] rede_no integer no describing GPIO configuration.
		 */
		void SET_REDE(int rede_no = 0) {
			this->rede_no = rede_no;
			switch (rede_no) {
				case 1:
					rede.RE_GPIOx = RE1_GPIO_Port;
					rede.RE_GPIO_PINx = RE1_Pin;
					rede.DE_GPIOx = DE1_GPIO_Port;
					rede.DE_GPIO_PINx = DE1_Pin;
					mux_pin = 6;
					break;
				case 2:
					rede.RE_GPIOx = RE2_GPIO_Port;
					rede.RE_GPIO_PINx = RE2_Pin;
					rede.DE_GPIOx = DE2_GPIO_Port;
					rede.DE_GPIO_PINx = DE2_Pin;
					mux_pin = 2;
					break;
				case 3:
					rede.RE_GPIOx = RE3_GPIO_Port;
					rede.RE_GPIO_PINx = RE3_Pin;
					rede.DE_GPIOx = DE3_GPIO_Port;
					rede.DE_GPIO_PINx = DE3_Pin;
					mux_pin = 1;
					break;
				case 4:
					rede.RE_GPIOx = RE4_GPIO_Port;
					rede.RE_GPIO_PINx = RE4_Pin;
					rede.DE_GPIOx = DE4_GPIO_Port;
					rede.DE_GPIO_PINx = DE4_Pin;
					mux_pin = 3;
					break;
#if defined(AWS_2_0)
				case 5:
					rede.RE_GPIOx = RE5_GPIO_Port;
					rede.RE_GPIO_PINx = RE5_Pin;
					rede.DE_GPIOx = DE5_GPIO_Port;
					rede.DE_GPIO_PINx = DE5_Pin;
					mux_pin = 5;
					break;
				case 6:
					rede.RE_GPIOx = RE6_GPIO_Port;
					rede.RE_GPIO_PINx = RE6_Pin;
					rede.DE_GPIOx = DE6_GPIO_Port;
					rede.DE_GPIO_PINx = DE6_Pin;
					mux_pin = 0;
					break;
				case 7:
					rede.RE_GPIOx = RE7_GPIO_Port;
					rede.RE_GPIO_PINx = RE7_Pin;
					rede.DE_GPIOx = DE7_GPIO_Port;
					rede.DE_GPIO_PINx = DE7_Pin;
					mux_pin = 4;
					break;
#endif
			}
			WRITE_REDE(GPIO_PIN_SET, 500);
		}

// baudrate for sensor
		/**
		 * @fn void SET_manual_baud(uint32_t manual_baud)
		 * @brief To set baudrate for sensor
		 */
		void SET_manual_baud(uint32_t manual_baud) {
			this->manual_baud = manual_baud;
		}
		/**
		 * @fn HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle(DynamicArray<size_type> *pData, uint32_t Timeout).
		 * @brief To be in ideal stage and receive mode until UART receive is not done.
		 * @param [in] pData A pointer to a Dynamic Array object which is going to receive incoming data from UART ,character by character.
		 * @param [in] Timeout intended timeout for UART_Receive
		 * @return HAL_status status about UART receieve.
		 */
		HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle(DynamicArray<size_type> *pData, uint32_t Timeout) {
			uint8_t *pdata8bits;
			uint16_t *pdata16bits;
			uint16_t uhMask;
			uint32_t tickstart;
			UNUSED(pdata16bits);
			/* Check that a Rx process is not already ongoing */
			if (RS485_USART->RxState == HAL_UART_STATE_READY) {
				if (pData == NULL) {
					return HAL_ERROR;
				}

				__HAL_LOCK(RS485_USART);
				RS485_USART->ErrorCode = HAL_UART_ERROR_NONE;
				RS485_USART->RxState = HAL_UART_STATE_BUSY_RX;
				RS485_USART->ReceptionType = HAL_UART_RECEPTION_TOIDLE;

				/* Init tickstart for timeout management */
				tickstart = HAL_GetTick();

				RS485_USART->RxXferSize = UINT16_MAX;
				RS485_USART->RxXferCount = UINT16_MAX;

				/* Computation of UART mask to apply to RDR register */
				UART_MASK_COMPUTATION(RS485_USART);
				uhMask = RS485_USART->Mask;

				/* In case of 9bits/No Parity transfer, pRxData needs to be handled as a uint16_t pointer */
				if ((RS485_USART->Init.WordLength == UART_WORDLENGTH_9B) && (RS485_USART->Init.Parity == UART_PARITY_NONE)) {
					pdata8bits = NULL;
					pdata16bits = (uint16_t*) pData->first();
				} else {
					pdata8bits = (uint8_t*) pData->first();
					pdata16bits = NULL;
				}

				__HAL_UNLOCK(RS485_USART);

				/* Initialize output number of received elements */

				/* as long as data have to be received */
				while (RS485_USART->RxXferCount > 0U) {
					/* Check if IDLE flag is set */
					if (__HAL_UART_GET_FLAG(RS485_USART, UART_FLAG_IDLE)) {
						/* Clear IDLE flag in ISR */
						__HAL_UART_CLEAR_FLAG(RS485_USART, UART_CLEAR_IDLEF);

						/* If Set, but no data ever received, clear flag without exiting loop */
						/* If Set, and data has already been received, this means Idle Event is valid : End reception */
						if (pData->size() > 0U) {
							RS485_USART->RxState = HAL_UART_STATE_READY;

							return HAL_OK;
						}
					}

					/* Check if RXNE flag is set */
					if (__HAL_UART_GET_FLAG(RS485_USART, UART_FLAG_RXNE)) {
						if (pdata8bits == NULL) {
							char ch = (RS485_USART->Instance->RDR & uhMask);
							pData->push_back(ch);
						} else {
							char ch = (RS485_USART->Instance->RDR & (uint8_t) uhMask);
							pData->push_back(ch);
						}
						RS485_USART->RxXferCount--;
					}

					/* Check for the Timeout */
					if (Timeout != HAL_MAX_DELAY) {
						if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U)) {
							RS485_USART->RxState = HAL_UART_STATE_READY;

							return HAL_TIMEOUT;
						}
					}
				}

				/* At end of Rx process, restore huart->RxState to Ready */
				RS485_USART->RxState = HAL_UART_STATE_READY;

				return HAL_OK;
			} else {
				return HAL_BUSY;
			}
		}

}
;

#endif /* RS485_H_ */

