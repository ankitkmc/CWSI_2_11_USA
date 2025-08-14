/*
 * double_to_string.h
 *
 *  Created on: May 23, 2023
 *      Author: dhanveer.singh
 */

#ifndef INC_DOUBLE_TO_STRING_H_
#define INC_DOUBLE_TO_STRING_H_

string d_t_h_s(uint32_t decimal) {
	const string hex_digits = "0123456789ABCDEF";
	string hex;
	uint32_t decimal_2 = decimal;
	if (decimal == 0) {
		hex = "0";
	}
	while (decimal > 0) {
		int digit = decimal % 16;
		hex = hex_digits[digit] + hex;
		decimal /= 16;
	}
	if (decimal_2 < 0xF) {
		hex = "0" + hex;
	}
	return hex;
}

string d_t_s(double value, int precision = 0, bool add_0 = 0) {

	string result;

	// Handle negative values
	if (value < 0) {
		result += "-";
		value = -value;
	}

	// Convert integer part
	unsigned long long intPart = static_cast<unsigned long long>(value);

	if (add_0 && intPart < 10) {
		result += "0";
	}
	result += to_string(intPart);

	// Convert decimal part
	if (precision > 0) {
		result += ".";
		double decimalPart = value - intPart;
		for (int i = 0; i < precision; ++i) {
			decimalPart *= 10;
			int digit = static_cast<int>(decimalPart);
			result += to_string(digit);
			decimalPart -= digit;
		}
	}
	return result;
}

double s_t_d(const std::string &str) {
	double result = 0.0;
	double factor = 1.0;
	bool hasDecimal = false;
	int decimalPlaces = 0;
	bool isNegative = false;
	bool hasLeadingZeros = true;

	// Handle negative sign
	if (!str.empty() && str[0] == '-') {
		isNegative = true;
	}

	for (char c : str) {
		if (c == '-') {
			continue;  // Skip the negative sign if present
		} else
			if (c == '.') {
				hasDecimal = true;
			} else
				if (c >= '0' && c <= '9') {
					if (hasDecimal) {
						++decimalPlaces;
						factor /= 10.0;
						result += (c - '0') * factor;
					} else {
						result = result * 10.0 + (c - '0');
					}
					if (hasLeadingZeros && c != '0') {
						hasLeadingZeros = false;
					}
				} else {
					// Invalid character, handle error as needed
					return 0.0;
				}
	}

	if (hasLeadingZeros) {
		return 0.0;  // Input string contains only leading zeros, consider it invalid
	}

	if (isNegative) {
		result = -result;
	}

	return result;
}
string d_t_b_s(uint32_t decimal, uint32_t highlight, uint32_t digits = 32) {
	string binary = "0b";
	if (decimal == 0) {
		binary += "0";
	}
	while (digits--) {
		if (highlight == digits) {
			binary += "(";
		}
		binary += d_t_s((decimal & (0x1 << digits)) > 0);
		if (highlight == digits) {
			binary += ")";
		}
	}

	return binary;
}

#endif /* INC_DOUBLE_TO_STRING_H_ */
