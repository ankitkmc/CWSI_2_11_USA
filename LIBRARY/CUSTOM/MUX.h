/*
 * MUX.h
 *
 *  Created on: Sep 11, 2023
 *      Author: Dhanveer
 */

#ifndef MUX_H_
#define MUX_H_

class MUX {
	PWR_PIN *s0, *s1, *s2;
public:

	MUX(PWR_PIN *MUX0, PWR_PIN *MUX1, PWR_PIN *MUX2) {
		s0 = MUX0;
		s1 = MUX1;
		s2 = MUX2;

	}
	void SELECT(uint16_t pin_no, uint32_t del = 0) {
		s0->SET(pin_no & 0x1, 0);
		s1->SET((pin_no >> 1) & 0x1, 0);
		s2->SET((pin_no >> 2) & 0x1, 0);
	}
};

#endif /* MUX_H_ */
