/*
 * I2CComm.cpp
 *
 *  Created on: Feb 10, 2019
 *      Author: osvaldo
 */

#include <I2CComm.h>
#define	LOCAL_BUF_SIZE	256

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
	uint8_t localBuf[LOCAL_BUF_SIZE];
	uint8_t bytesRead = (uint8_t) Wire.readBytes(localBuf, howMany);

	uint8_t writeIdxBck = arCtrl->writeIdx;
	for(int i = 0; i < bytesRead; i++)
	{
		if (arCtrl->writeIdx == 255) arCtrl->writeIdx = 0;
		if (arCtrl->writeIdx == arCtrl->readIdx)
		{
			arCtrl->writeIdx = writeIdxBck;
			arCtrl->writeBuf[0] = 0x02;
			arCtrl->writeBuf[1] = I2CCMD_NACK;
			arCtrl->writeBufLen = 2;
			return;
		}
		arCtrl->readBuf[arCtrl->writeIdx] = localBuf[i];
		Serial.print(arCtrl->toHex(localBuf[i]));
		arCtrl->writeIdx++;
	}
	Serial.println(" - New message received");
	return;

//	memset(writeBuffer, '\0', sizeof(writeBuffer));
//	bufLen = 0;
//	Serial.print("The controller is");
//	Serial.print((arCtrl->getInitialized() ? " " : " not "));
//	Serial.println("initialized");
//	if (arCtrl->getInitialized())
//	{
//		bufLen = arCtrl->handleI2CCommand((const unsigned char *)&readBuffer[0], &writeBuffer[0]);
//	}
//	else if (readBuffer[1] != I2CCMD_INITIALIZE_UNIT)
//	{
//		writeBuffer[0] = 0x02;
//		writeBuffer[1] = I2CCMD_NACK;
//		bufLen = 2;
//	}
//	else
//	{
//		bufLen = arCtrl->initializeControls((const unsigned char *)&readBuffer[0], &writeBuffer[0]);
//		arCtrl->setup(millis());
//		arCtrl->setInitialized(true);
//		writeBuffer[0] = 0x02;
//		writeBuffer[1] = I2CCMD_ACK;
//		bufLen = 2;
//	}
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
//		Serial.print("Sending response pending. Length ");
//		Serial.println(arCtrl->writeBufLen);
//		Serial.println("Message content: ");
//		for(int y = 0; y < arCtrl->writeBufLen; y++)
//		{
//			;
//			if (isprint(arCtrl->writeBuf[y]))
//			{
//				Serial.print("   ");
//				Serial.print(arCtrl->writeBuf[y]);
//				Serial.print(" ");
//			}
//			else
//			{
//				Serial.print(arCtrl->toHex(arCtrl->writeBuf[y]));
//				Serial.print(" ");
//			}
//		}
//		Serial.println();
		Serial.println("Response pending:");
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
		Wire.write(arCtrl->writeBuf, arCtrl->writeBufLen);
		memset(arCtrl->writeBuf, '\0', arCtrl->writeBufLen);
		arCtrl->writeBufLen = 0;
	}
}


//I2CComm::I2CComm(void (*receiveEvent)(int), void (*sendEvent)()) {
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

