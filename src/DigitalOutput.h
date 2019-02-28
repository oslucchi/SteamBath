/*
 * ValveBiState.h
 *
 *  Created on: Feb 18, 2019
 *      Author: osvaldo
 */

#ifndef DIGITALOUTPUT_H_
#define DIGITALOUTPUT_H_

#include <Command.h>

class DigitalOutput: public Command {
public:
	DigitalOutput();
	virtual ~DigitalOutput();
};

#endif /* DIGITALOUTPUT_H_ */
