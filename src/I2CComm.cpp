/*
 * I2CComm.cpp
 *
 *  Created on: Feb 10, 2019
 *      Author: osvaldo
 */

#include <I2CComm.h>
#define	LOCAL_BUF_SIZE	128

ArduinoControl * arCtrl;


void clearChannel()
{
	Serial.println("Clearing channel...");

	while( Wire.available())
	{
		byte b = Wire.read();
		Serial.print(" ");
		Serial.print(arCtrl->toHex(b));
	}
	Serial.println();
}

void receiveEvent(int howMany)
{
	uint8_t localBuf[64];
	uint8_t bytesRead = (uint8_t) Wire.readBytes(localBuf, howMany);

	uint8_t writeIdxBck = arCtrl->writeIdx;
	for(int i = 0; i < bytesRead; i++)
	{
		if ((arCtrl->writeIdx == arCtrl->readIdx - 1) ||
			((arCtrl->writeIdx == LOCAL_BUF_SIZE - 1) && (arCtrl->readIdx == 0)))
		{
			arCtrl->writeIdx = writeIdxBck;
			arCtrl->writeBuf[0] = 0x02;
			arCtrl->writeBuf[1] = I2CCMD_NACK;
			arCtrl->writeBufLen = 2;
			return;
		}

		if (arCtrl->writeIdx == LOCAL_BUF_SIZE - 1) arCtrl->writeIdx = 0;
		arCtrl->readBuf[arCtrl->writeIdx++] = localBuf[i];
		Serial.print(arCtrl->toHex(localBuf[i]));
	}
	Serial.println(" - New message received");
	sprintf((char *) localBuf, "readIdx %d - writeIdx %d", arCtrl->readIdx, arCtrl->writeIdx);
	Serial.println((char *) localBuf);
	return;
}

void sendEvent()
{
	if (arCtrl->writeBufLen == 0)
	{
		Serial.println("Answer not ready");
		uint8_t buffer[2];
		buffer[0] = 2;
		buffer[1] = I2CCMD_NOT_READY;
		Wire.write(buffer, 2);
	}
	else
	{
		Serial.print("Response pending (len ");
		Serial.print(arCtrl->writeBufLen);
		Serial.println("):");
		for(int y = 0; y < arCtrl->writeBufLen; y++)
		{
			;
			if (isprint(arCtrl->writeBuf[y]))
			{
				Serial.print("   ");
				Serial.print(arCtrl->writeBuf[y]);
				Serial.print(" ");
			}
			else
			{
				Serial.print(arCtrl->toHex(arCtrl->writeBuf[y]));
				Serial.print(" ");
			}
		}
		Serial.println();
		Wire.write((char *) arCtrl->writeBuf, (size_t) arCtrl->writeBufLen);
		memset(arCtrl->writeBuf, '\0', LOCAL_BUF_SIZE);
		arCtrl->writeBufLen = 0;
	}
}

I2CComm::I2CComm(ArduinoControl * control) {
	arCtrl = control;
	Wire.begin(I2C_BUS_ADDRESS);
	Serial.print("On the i2c bus with address ");
	Serial.println(I2C_BUS_ADDRESS);
	clearChannel();
}

void I2CComm::setCallback()
{
	Wire.onReceive(receiveEvent);
	Wire.onRequest(sendEvent);
}

I2CComm::~I2CComm() {
}

