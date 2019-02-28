// Do not remove the include below
#include "steamBath.h"

I2CComm *i2c;
ArduinoControl *ac = new ArduinoControl();
unsigned long startTime;
uint8_t status = 1;

//The setup function is called once at startup of the sketch
void setup()
{
	//	i2c = new I2CComm(&receiveEvent, &sendEvent);
	//	Serial.print("On the i2c bus with address ");
	//	Serial.println(I2C_BUS_ADDRESS);
	//	Wire.onReceive(receiveEvent);
	//	Wire.onRequest(sendEvent);

	Serial.begin(57600);
	Serial.println("Starting controls");
	i2c = new I2CComm(ac);
	i2c->setCallback();
	startTime = millis();
	digitalWrite(LED_BUILTIN, status);
}

// The loop function is called in an endless loop
void loop()
{
	unsigned long now = millis();
	ac->loop(now);
	if (now - startTime > 1000)
	{
		digitalWrite(LED_BUILTIN, (status = !status));
		startTime = now;
	}

}


