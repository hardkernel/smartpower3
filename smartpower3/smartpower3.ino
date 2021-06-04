#include "smartpower3.h"
#include <Wire.h>
#include <SPIFFS.h>

void setup(void) {
	Serial.begin(115200);
	pinMode(27, OUTPUT);
	pinMode(14, OUTPUT);
	digitalWrite(27, HIGH);
	digitalWrite(14, HIGH);
	I2CA.begin(15, 4);
	I2CB.begin(21, 22);
	PAC.begin(&I2CB);
	screen.begin(&I2CA);

	initEncoder(&dial);
	initPAC1933();
	//write8(&I2CA, 0x04, 0xff);

	xTaskCreate(powerTask, "Read Power", 2000, NULL, 1, NULL);
	xTaskCreate(screenTask, "Draw Screen", 2000, NULL, 1, NULL);
	xTaskCreate(inputTask, "Input Task", 2000, NULL, 1, NULL);
	pinMode(25, INPUT_PULLUP);
	attachInterrupt(25, isr_stp, FALLING);
}

void isr_stp()
{
	Serial.println("Hello isr stp");
}

void initPAC1933(void)
{
	Serial.println("Initialize PAC1933");
	PAC.UpdateProductID();
	Serial.print(PAC.ProductID, HEX);
	Serial.print("\n Manufacturer ID: ");
	PAC.UpdateManufacturerID();
	Serial.print(PAC.ManufacturerID, HEX);
	Serial.print("\n Revision     ID: ");
	PAC.UpdateRevisionID();
	Serial.print(PAC.RevisionID, HEX);
}

void updatePowerSense3(void)
{
	PAC.UpdateVoltageSense3();
	PAC.UpdateCurrentSense3();
	PAC.UpdatePowerSense3();
	/*
	PAC.UpdatePowerAcc();
	*/
}

void updatePowerSense2(void)
{
	PAC.UpdateVoltageSense2();
	PAC.UpdateCurrentSense2();
	PAC.UpdatePowerSense2();
	/*
	PAC.UpdatePowerAcc();
	*/
}

void updatePowerSense1(void)
{
	PAC.UpdateVoltageSense1();
	PAC.UpdateCurrentSense1();
	PAC.UpdatePowerSense1();
	/*
	PAC.UpdatePowerAcc();
	*/
}

void powerTask(void *parameter)
{
	uint16_t volt, ampere, watt;
	for (;;) {
		updatePowerSense2();
		volt = (uint16_t)(PAC.Voltage);
		ampere = (uint16_t)(PAC.Current);
		watt = (uint16_t)(PAC.Power);
		screen.pushPower(volt, ampere, watt, 0);
		updatePowerSense3();
		volt = (uint16_t)(PAC.Voltage);
		ampere = (uint16_t)(PAC.Current);
		watt = (uint16_t)(PAC.Power);
		screen.pushPower(volt, ampere, watt, 1);

		vTaskDelay(100);
	}
}

void screenTask(void *parameter)
{
	for (;;) {
		screen.drawScreen();
		vTaskDelay(100);
	}
}

void inputTask(void *parameter)
{
	for (;;) {
		cur_time = millis();
		for (int i = 0; i < 4; i++) {
			if (button[i].checkPressed() == true)
				screen.getBtnPress(i, cur_time);
		}
		if (dial.cnt != 0) {
			screen.countDial(dial.cnt, dial.direct, cur_time);
			dial.cnt = 0;
		}
		screen.setTime(cur_time);
		vTaskDelay(100);
	}
}

void loop() {
	//get_memory_info();
	//get_i2c_slaves();
	delay(100);
}

void get_memory_info(void)
{
	Serial.printf("Heap : %d, Free Heap : %d\n", ESP.getHeapSize(), ESP.getFreeHeap());
	Serial.printf("Stack High Water Mark %d\n", uxTaskGetStackHighWaterMark(NULL));
	Serial.printf("Psram : %d, Free Psram : %d\n", ESP.getPsramSize(), ESP.getFreePsram());
}

void get_i2c_slaves(TwoWire *theWire)
{
	byte error, address;
	int nDevices;

	Serial.println("Scanning...");

	nDevices = 0;
	for (address = 1; address < 127; address++) {
		theWire->beginTransmission(address);
		error = theWire->endTransmission();

		if (error == 0) {
			Serial.print("I2C device found at address 0x");
			if (address < 16)
				Serial.print("0");
			Serial.print(address, HEX);
			Serial.println("  !");
			nDevices++;
		} else if (error == 4) {
			Serial.println("Unknown error at address 0x");
			if (address < 16)
				Serial.print("0");
			Serial.println(address, HEX);
		}
	}
	if (nDevices == 0)
		Serial.println("No I2C devices found");
	else
		Serial.println("done");
}
