/*
 * LedStripeHandler.cpp
 *
 *  Created on: Feb 10, 2019
 *      Author: osvaldo
 */
#include <LedRGB.h>
#include "I2CComm.h"

unsigned long now = 0;

LedRGB::LedRGB()
{
}

void LedRGB::setup(unsigned long now)
{
	leds[RED].timer = timerManager->getNewTimer("RED");
	leds[RED].timer->setDuration(leds[RED].speed);
	leds[GREEN].timer = timerManager->getNewTimer("GREEN");
	leds[GREEN].timer->setDuration(leds[RED].speed);
	leds[BLUE].timer = timerManager->getNewTimer("BLUE");
	leds[BLUE].timer->setDuration(leds[RED].speed);

	if (mode == MODE_AUTO)
	{
		for(int i = 0; i < 3; i++)
		{
			leds[i].timer->start(now);
		}
	}
}

void LedRGB::loop(unsigned long now)
{
	for(int i = 0; i < 3; i++)
	{
		switch(mode)
		{
		case MODE_AUTO:
			if (leds[i].timer->getIsExpired())
			{
//				Serial.print("Led "); Serial.print(leds[i].pin); Serial.print(" value "); Serial.println(leds[i].actualValue * status);
				pwmBrightness(i);
				leds[i].timer->setDuration(leds[i].speed);
				leds[i].timer->restart(now);
			}
			break;

		case MODE_MANUAL:
			break;
		default:
			Serial.println("LedRGB in unknown mode");
		}
		analogWrite(leds[i].pin, leds[i].actualValue * status);
	}
}

int LedRGB::getActualValues(unsigned char *response)
{
	response[RED] = leds[RED].actualValue;
	response[GREEN] = leds[GREEN].actualValue;
	response[BLUE] = leds[BLUE].actualValue;
	return(3);
}

void LedRGB::handleCommand(const unsigned char * command, unsigned char * response)
{
	bool switchOnFlag;

	switch(command[0])
	{
	case 0x01:
		Serial.println("I2CCMD_GET");
		response[0] = 5;
		response[1] = I2CCMD_ACK;
		response[2] = leds[RED].actualValue;
		response[3] = leds[GREEN].actualValue;
		response[4] = leds[BLUE].actualValue;
		break;

	case 0x02:
		Serial.println("LedRGB: I2CCMD_SET_MANUAL");
		turnManualOn();
		switchOff();
		response[0] = 2;
		response[1] = I2CCMD_ACK;
		break;

	case 0x03:
		Serial.println("LedRGB: I2CCMD_SET_AUTO");
		setUpperBound((uint8_t *)&command[1]);
		setLowerBound((uint8_t *)&command[4]);
		setSpeed((uint8_t *)&command[7]);
		setActualValue((uint8_t *)&command[10]);
		turnAutoOn();
		response[0] = 2;
		response[1] = I2CCMD_ACK;
		break;

	case 0x04:
		Serial.println("LedRGB: I2CCMD_SET_BRIGHTNESS");
		leds[RED].actualValue = command[1];
		leds[GREEN].actualValue = command[2];
		leds[BLUE].actualValue = command[3];
		response[0] = 2;
		response[1] = I2CCMD_ACK;
		break;

	case 0x05:
		Serial.println("LedRGB: I2CCMD_SWITCH_ON_OFF");
		switchOnFlag = (bool) command[1];
		if (switchOnFlag)
			switchOn();
		else
			switchOff();
		response[0] = 2;
		response[1] = I2CCMD_ACK;
		break;

	case 0x06:
		Serial.println("LedRGB: I2CCMD_SET_SPEED");
		leds[RED].speed = command[1];
		leds[GREEN].speed = command[2];
		leds[BLUE].speed = command[3];
		response[0] = 2;
		response[1] = I2CCMD_ACK;
		break;

	case 0x07:
		Serial.println("LedRGB: I2CCMD_RESET_TIMERS");
		leds[RED].timer->restart(now);
		leds[GREEN].timer->restart(now);
		leds[BLUE].timer->restart(now);
		response[0] = 2;
		response[1] = I2CCMD_ACK;
		break;

	case 0x08:
		Serial.println("LedRGB: I2CCMD_FADE");
		int direction;
		if (command[1] == 0)
		{
			direction = -1;
		}
		else
		{
			direction = 1;
		}
		leds[RED].direction = direction;
		leds[GREEN].direction = direction;
		leds[BLUE].direction = direction;
		fading = true;
		turnAutoOn();
		response[0] = 2;
		response[1] = I2CCMD_ACK;
		break;
	}
}

void LedRGB::switchOn()
{
	status = POWER_ON;
}

void LedRGB::switchOff()
{
	status = POWER_OFF;
}

void LedRGB::turnManualOn()
{
	mode = MODE_MANUAL;
}

void LedRGB::turnAutoOn()
{
	mode = MODE_AUTO;
	for(int i = 0; i < 3; i++)
	{
		Serial.print("Turning on speed "); Serial.println(leds[i].speed);
		leds[i].timer->setDuration(leds[i].speed);
		leds[i].timer->start(now);
	}
}

void LedRGB::setUpperBound(uint8_t* ub)
{
	leds[RED].upperBound = ub[0];
	leds[GREEN].upperBound = ub[1];
	leds[BLUE].upperBound = ub[2];
}

void LedRGB::setLowerBound(uint8_t* lb)
{
	leds[RED].lowerBound = lb[0];
	leds[GREEN].lowerBound = lb[1];
	leds[BLUE].lowerBound = lb[2];
}

void LedRGB::setSpeed(uint8_t* s)
{
	leds[RED].speed = s[0];
	leds[RED].timer->setDuration(s[0]);
	leds[RED].timer->restart(millis());
	leds[GREEN].speed = s[1];
	leds[GREEN].timer->setDuration(s[1]);
	leds[GREEN].timer->restart(millis());
	leds[BLUE].speed = s[2];
	leds[BLUE].timer->setDuration(s[2]);
	leds[BLUE].timer->restart(millis());
}

void LedRGB::setActualValue(uint8_t* sv)
{
	leds[RED].actualValue = sv[0];
	leds[GREEN].actualValue = sv[1];
	leds[BLUE].actualValue = sv[2];
}

void LedRGB::setPin(uint8_t * pin)
{
	leds[RED].pin = pin[0];
	leds[GREEN].pin = pin[1];
	leds[BLUE].pin = pin[2];

}

void LedRGB::fade()
{
	fading = true;
}

void LedRGB::pwmBrightness(uint8_t led)
{
	if (leds[led].actualValue > leds[led].upperBound) leds[led].actualValue = leds[led].upperBound;
	if (leds[led].actualValue < leds[led].lowerBound) leds[led].actualValue = leds[led].lowerBound;

	if (leds[led].actualValue >= leds[led].upperBound && leds[led].direction == 1)
	{
		leds[led].actualValue = leds[led].upperBound;
		if (fading)
		{
			turnManualOn();
			fading = false;
		}
		else
			leds[led].direction = -1;
	}
	else if (leds[led].actualValue <= leds[led].lowerBound && leds[led].direction == -1)
	{
		leds[led].actualValue = leds[led].lowerBound;
		if (fading)
		{
			turnManualOn();
			fading = false;
		}
		else
			leds[led].direction = 1;
	}
	leds[led].actualValue += leds[led].direction;
};

