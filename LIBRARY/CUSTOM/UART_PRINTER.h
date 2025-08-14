/*
 * UART_PRINTER.h
 *
 *  Created on: Aug 21, 2023
 *      Author: dhanveer.singh
 */

#ifndef UART_PRINTER_H_
#define UART_PRINTER_H_

class UART_PRINTER {

		UART_HandleTypeDef *uptr;  //!< A Pointer pointing to UART_HandleTypeDef object.
		string Pass;  //!< A string value representing intended password
	public:

		UART_PRINTER(UART_HandleTypeDef *uptr_i, string Pass_i) {

			uptr = uptr_i;
			Pass = Pass_i;
		}

		void Print(uint8_t *out, uint32_t out_size, string name) {
			if (is_uart_print()) {
				Print("\n\rARR - " + name + " ");
				for (uint32_t var = 0; var < out_size; var++) {
					Print(d_t_h_s(*(out + var)));
					Print(" ");
				}
				Print("\n\r");
			}
		}
		void Print(uint16_t out, uint32_t out_size, string name) {
			if (is_uart_print()) {
				Print("\n\rARR - " + name + " ");
				for (uint32_t var = 0; var < out_size; var++) {
					Print((out + var));
					Print(" ");
				}
				Print("\n\r");
			}
		}
		/**
		 * @fn 	string Read()
		 * @brief To take input string from user through UART protocol
		 */
		string Read(uint32_t timeout) {
			//To ask user to enter value to be changed
			string read_string;
			if (is_uart_print()) {
				HAL_UART_Init(uptr);
				HAL_UARTEx_ReceiveToIdle(uptr, read_string, { timeout });
			}
			while (read_string.back() == '\n' || read_string.back() == '\r') {
				read_string.pop_back();
			}
			return read_string;
		}

		/**
		 * @fn 	void Print(double out)
		 * @brief To Print Data into serial monitor with input as a double value.
		 */
		void Print(double out, double decimals = 5) {
			if (is_uart_print()) {
				Print(d_t_s(out, decimals));
			}
		}
		/**
		 * @fn 	void Print(string out = NULL)
		 * @brief To Print Data into serial monitor with input as a string value.
		 */
		void Print(string out = NULL) {
//			PrintForce(d_t_s(is_uart_print(), 0));
			if (is_uart_print()) {
				HAL_UART_Init(uptr);
				HAL_UART_Transmit(uptr, (uint8_t*) out.c_str(), out.size(), UINT32_MAX);

			}
		}

		/**
		 * @fn 	void Print(DynamicArray<uint8_t> temp)
		 * @brief To Print Data into serial monitor with input as a Dynamic Array object.
		 */
		void Print(DynamicArray<uint8_t> &temp) {
			if (is_uart_print()) {
				if (temp.is_empty()) {
					Print("EMPTY FRAME");
				} else {
					for (uint32_t iterator = 0; iterator < temp.size(); iterator++) {
						double temp_var = *(temp.at(iterator));
						Print(d_t_h_s(temp_var));
						Print(" ");
					};
				}
				Print("\n\r");
			}
		}

		string get_pass() {
			return Pass;
		}

};

#endif /* UART_PRINTER_H_ */
