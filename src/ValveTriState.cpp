/*
 * ValveTriState.cpp
 *
 *  Created on: Feb 18, 2019
 *      Author: osvaldo
 */

#include "Arduino.h"
#include <ValveTriState.h>

ValveTriState::ValveTriState() {
	timer = (Timer **) malloc(sizeof (Timer *));
	timer[0] = new Timer();
	timer[1] = new Timer();
	way = 0;
}

ValveTriState::~ValveTriState() {
	// TODO Auto-generated destructor stub
}

void ValveTriState::switchOn()
{
	if (way == VALVE_WAY_A)
	{
		digitalWrite(pin[VALVE_WAY_B], 0);
		digitalWrite(pin[VALVE_WAY_A], 1);
	}
	else
	{
		digitalWrite(pin[VALVE_WAY_A], 0);
		digitalWrite(pin[VALVE_WAY_B], 1);
	}
}

void ValveTriState::switchOff()
{
	digitalWrite(pin[VALVE_WAY_B], 0);
	digitalWrite(pin[VALVE_WAY_A], 0);
}

void ValveTriState::setTimer(unsigned char * t)
{
	timer[0]->setDuration(t);
}

void ValveTriState::setWay(unsigned char w)
{
	way = w;
}

void ValveTriState::autoOn()
{
	switchOn();
	timer[0]->start(millis());
	isAuto = true;
}

void ValveTriState::setup(unsigned long now)
{
	Serial.print(now);
}

void ValveTriState::loop(unsigned long now)
{
	if (timer[0]->getIsExpired() && timer[0]->getIsFront())
	{
		Serial.print(now);
		switchOff();
		isAuto = false;
	}
}
