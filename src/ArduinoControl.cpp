/*
 * ArduinoControl.cpp
 *
 *  Created on: Feb 16, 2019
 *      Author: osvaldo
 */

#include "Arduino.h"
#include <ArduinoControl.h>
#include <LedRGB.h>
#include <ValveTriState.h>
#include <DigitalOutput.h>

ArduinoControl::ArduinoControl() {
	uint8_t pin[3], upperBound[3], lowerBound[3], actualValue[3];

	initialized = false;
	Serial.println("ArduinoControl: registering controls");

	pin[RED] = 11;
	pin[GREEN] = 10;
	pin[BLUE] = 9;
	upperBound[RED] = 255;
	upperBound[GREEN] = 255;
	upperBound[BLUE] = 255;
	lowerBound[RED] = 0;
	lowerBound[GREEN] = 0;
	lowerBound[BLUE] = 0;
	actualValue[RED] = 0;
	actualValue[GREEN] = 0;
	actualValue[BLUE] = 0;
	steamLigthsSensors.addCommand(CTRLID_RGBSTRIPE, (uint8_t) TYPE_LED_RGB, pin,
								  upperBound, lowerBound, actualValue, &timerManager);

	pin[RED] = 6;
	pin[GREEN] = 5;
	pin[BLUE] = 3;
	upperBound[RED] = 255;
	upperBound[GREEN] = 255;
	upperBound[BLUE] = 255;
	lowerBound[RED] = 0;
	lowerBound[GREEN] = 0;
	lowerBound[BLUE] = 0;
	actualValue[RED] = 0;
	actualValue[GREEN] = 0;
	actualValue[BLUE] = 0;
	steamLigthsSensors.addCommand(CTRLID_STARRYSKY, (uint8_t) TYPE_LED_RGB, pin,
								  upperBound, lowerBound, actualValue, &timerManager);

	pin[VALVE_WAY_A] = 7;
	pin[VALVE_WAY_B] = 8;
	pin[2] = -1;
	upperBound[0] = 1;
	upperBound[1] = -1;
	upperBound[2] = -1;
	lowerBound[0] = 0;
	lowerBound[1] = -1;
	lowerBound[2] = 1;
	actualValue[0] = 0;
	actualValue[1] = -1;
	actualValue[2] = -1;
//	steamLigthsSensors.addCommand(CTRLID_STEAM_WATER_IN, (uint8_t) TYPE_VALVE_TRISTATE, pin,
//								  upperBound, lowerBound, actualValue, &timerManager);

	pin[0] = 12;
	pin[1] = -1;
//	steamLigthsSensors.addCommand(CTRLID_STEAM_WATER_OUT, (uint8_t) TYPE_DIGITAL_OUT, pin, upperBound, lowerBound, actualValue);

	pin[0] = 4;
//	steamLigthsSensors.addCommand(CTRLID_STEAM_POWER, (uint8_t) TYPE_DIGITAL_OUT, pin, upperBound, lowerBound, actualValue);

	pin[0] = 13;
//	steamLigthsSensors.addCommand(CTRLID_HUMIDITY_SENSOR, (uint8_t) TYPE_DIGITAL_IN, pin, upperBound, lowerBound, actualValue);
}

ArduinoControl::~ArduinoControl() {
	// TODO Auto-generated destructor stub
}

char* ArduinoControl::toHex(uint8_t a)
{
	static char hex[6];
	hex[0] = '0';
	hex[1] = 'x';
	hex[2] = (char) (a / 16);
	if (hex[2] >= 10)
		hex[2] += 55;
	else
		hex[2] += 48;
	hex[3] = (char) (a % 16);
	if (hex[3] >= 10)
		hex[3] += 55;
	else
		hex[3] += 48;
	hex[4] = ' ';
	hex[5] = '\0';
	return(hex);
}

void ArduinoControl::setup(unsigned long now)
{
	cmdList *current = steamLigthsSensors.getController();
	while((current = current->next) != nullptr)
	{
		current->cmd->setup(now);
	}
}

void ArduinoControl::loop(unsigned long now)
{
	if (writeIdx != readIdx)
	{
		handleI2CCommand();
	}
	timerManager.update(now);

	cmdList *current = steamLigthsSensors.getController();
	while((current = current->next) != nullptr)
	{
		current->cmd->loop(now);
	}
}

Command* ArduinoControl::getCommand(unsigned char ctrlId)
{
	cmdList *current = steamLigthsSensors.getController();
	while((current = current->next) != nullptr)
	{
		if (current->cmd->ctrlId == ctrlId)
			return(current->cmd);
	}
	return(nullptr);
}

int ArduinoControl::handleI2CCommand()
{
	char dbgBuffer[80];
	sprintf((char *) dbgBuffer, "Received command. Current idxs read %d - write %d", readIdx, writeIdx);
	Serial.println((char *) dbgBuffer);

	uint8_t cmdBuf[32];

	uint8_t cmdLen = readBuf[readIdx++];
	sprintf((char *) dbgBuffer, "Msg len %d. Dumping content", cmdLen);
	Serial.println((char *) dbgBuffer);

	for(int i = 1; i < cmdLen; i++)
	{
		if (readIdx == LOCAL_BUF_SIZE - 1) readIdx = 0;
		cmdBuf[i - 1] = readBuf[readIdx++];
		Serial.print(toHex(cmdBuf[i - 1]));
	}
	Serial.println();

	Command* ctrl = getCommand(cmdBuf[0]);

	if (ctrl == nullptr)
	{
		Serial.print("Command ");
		Serial.print(cmdBuf[0]);
		Serial.println(" not found");
		writeBuf[0] = (uint8_t) 0x02;
		writeBuf[1] = (uint8_t) I2CCMD_NACK;
	}
	else
	{
		ctrl->handleCommand(&cmdBuf[1], writeBuf);
	}
	writeBufLen = writeBuf[0];
	return writeBuf[0];
}

int ArduinoControl::initializeControls(const unsigned char * command, unsigned char * response)
{
	Command* ctrl;
	int offset = 0;
	unsigned long now = millis();
//	RGB Stripe
//		byte 2 set mode auto / manual (0x00 / 0x01)
//		bytes 3-5 set point (RGB)
//		bytes 6-8 upper limit (RGB)
//		bytes 9-11 lower limit (RGB)
//		bytes 12-14 speed (RGB)
//		bytes 15-17 starting point (RGB)
//		byte 18 swith off/on (0x00/0x01)
	ctrl = getCommand(CTRLID_RGBSTRIPE);
	Serial.println("QUI");

	((LedRGB *)ctrl)->setUpperBound((unsigned char *) &command[offset + 2]);
	((LedRGB *)ctrl)->setLowerBound((unsigned char *) &command[offset + 5]);
	((LedRGB *)ctrl)->setSpeed((unsigned char *) &command[offset + 8]);
	((LedRGB *)ctrl)->setActualValue((unsigned char *) &command[offset + 11]);
	(command[offset + 14] == 0 ? ((LedRGB *)ctrl)->turnManualOn() : ((LedRGB *)ctrl)->turnAutoOn());
	(command[offset + 15] == 0 ? ((LedRGB *)ctrl)->switchOff() : ((LedRGB *)ctrl)->switchOff());
	((LedRGB *)ctrl)->setup(now);
//	Serial.println("RGBSTRIPE Set received:");
//	Serial.print("\tsetPoint: ");
//	Serial.print( (int)((LedRGB *)ctrl)->getLeds()[0].setPoint);
//	Serial.print(" ");
//	Serial.print( (int)((LedRGB *)ctrl)->getLeds()[1].setPoint);
//	Serial.print(" ");
//	Serial.println( (int)((LedRGB *)ctrl)->getLeds()[2].setPoint);
//	Serial.print("\tlowerBound: ");
//	Serial.print( (int)((LedRGB *)ctrl)->getLeds()[0].getLowerBound());
//	Serial.print(" ");
//	Serial.print( (int)((LedRGB *)ctrl)->getLeds()[1].getLowerBound());
//	Serial.print(" ");
//	Serial.println( (int)((LedRGB *)ctrl)->getLeds()[2].getLowerBound());
//	Serial.print("\tupperBound: ");
//	Serial.print( (int)((LedRGB *)ctrl)->getLeds()[0].getUpperBound());
//	Serial.print(" ");
//	Serial.print( (int)((LedRGB *)ctrl)->getLeds()[1].getUpperBound());
//	Serial.print(" ");
//	Serial.println( (int)((LedRGB *)ctrl)->getLeds()[2].getUpperBound());
//	Serial.print("\tspeed: ");
//	Serial.print( (int)((LedRGB *)ctrl)->getLeds()[0].getSpeed());
//	Serial.print(" ");
//	Serial.print( (int)((LedRGB *)ctrl)->getLeds()[1].getSpeed());
//	Serial.print(" ");
//	Serial.println( (int)((LedRGB *)ctrl)->getLeds()[2].getSpeed());
//	Serial.print("\tstartingValue: ");
//	Serial.print( (int)((LedRGB *)ctrl)->getLeds()[0].getStartingValue());
//	Serial.print(" ");
//	Serial.print( (int)((LedRGB *)ctrl)->getLeds()[1].getStartingValue());
//	Serial.print(" ");
//	Serial.println( (int)((LedRGB *)ctrl)->getLeds()[2].getStartingValue());

//	Starry sky
//		byte 2 set mode auto / manual (0x00 / 0x01)
//		bytes 3-5 set point (RGB)
//		bytes 6-8 upper limit (RGB)
//		bytes 9-12 lower limit (RGB)
//		bytes 13-14 speed (RGB)
//		bytes 15-17 starting point (RGB)
//		byte 18 swith off/on (0x00/0x01)

//	offset += 18;
//	ctrl = getCommand(CTRLID_STARRYSKY);
//	(command[offset + 2] == 0 ? ((LedRGB *)ctrl)->turnManualOn() : ((LedRGB *)ctrl)->turnAutoOn());
//	((LedRGB *)ctrl)->setSetPoint(&command[offset + 3]);
//	((LedRGB *)ctrl)->setUpperBound((unsigned char *) &command[offset + 6]);
//	((LedRGB *)ctrl)->setLowerBound((unsigned char *) &command[offset + 9]);
//	((LedRGB *)ctrl)->setSpeed((unsigned char *) &command[offset + 12]);
//	((LedRGB *)ctrl)->setStartingValue((unsigned char *) &command[offset + 15]);
//	(command[offset + 18] == 0 ? ((LedRGB *)ctrl)->switchOff() : ((LedRGB *)ctrl)->switchOff());

//	Valve steamWaterIn
//		byte 2 which way A / B (0x01 / 0x02)
//		byte 3 auto/manual (0x00 / 0x01)
//		byte 3-4 timer
//			2 bits timer units (0x00 seconds, 0x01 minutes, 0x10 hours, 0x11 days)
//			7 bits # of units
//			7 bits # decimals to units (e.g. if units is second, this is 1/10 of seconds, if is minutes, this is seconds etc)

//	offset += 18;
//	ctrl = getCommand(CTRLID_STEAM_WATER_IN);
//	((ValveTriState *)ctrl)->setWay(command[offset + 2]);
//	((ValveTriState *)ctrl)->setManual(command[offset + 3]);
//	((ValveTriState *)ctrl)->setTimer((unsigned char *)&command[offset + 4]);

//	Valve steamWaterOut
//		byte 2-3 timer
//			2 bits timer units (0x00 seconds, 0x01 minutes, 0x10 hours, 0x11 days)
//			7 bits # of units
//			7 bits # decimals to units (e.g. if units is second, this is 1/10 of seconds, if is minutes, this is seconds etc)

//	Humidity sensor
//		byte 2-5 counter value (2 most significant / 5 less significant)

	response[1] = I2CCMD_ACK;
	response[0] = 2;
	initialized = true;
	return(2);
}

void ArduinoControl::setInitialized(bool flag)
{
	initialized = flag;
}

bool ArduinoControl::getInitialized()
{
	return initialized;
}
