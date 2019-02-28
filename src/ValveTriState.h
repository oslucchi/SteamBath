/*
 * ValveTriState.h
 *
 *  Created on: Feb 18, 2019
 *      Author: osvaldo
 */

#ifndef VALVETRISTATE_H_
#define VALVETRISTATE_H_

#include <Command.h>

#define	VALVE_WAY_A	0x00
#define	VALVE_WAY_B	0x01

class ValveTriState: public Command {
	unsigned char way;
	bool isAuto;
public:
	ValveTriState();
	virtual ~ValveTriState();
	void switchOn();
	void switchOff();
	void setTimer(unsigned char *);
	void setWay(unsigned char);
	void autoOn();
	void setup(unsigned long now);
	void loop(unsigned long now);
};

#endif /* VALVETRISTATE_H_ */
