/*
 * Input.h
 *
 *  Created on: Feb 18, 2019
 *      Author: osvaldo
 */

#ifndef DIGITALINPUT_H_
#define DIGITALINPUT_H_

#include <Command.h>

class DigitalInput: public Command {
public:
	DigitalInput();
	virtual ~DigitalInput();
};

#endif /* DIGITALINPUT_H_ */
