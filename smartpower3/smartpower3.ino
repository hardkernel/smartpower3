#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Microchip_PAC193x.h>
#include "screen.h"
#include "inputmanager.h"

#include <Wire.h>

#define BUTTON_MENU 36
#define BUTTON_CH0 39
#define BUTTON_CH1 34
#define BUTTON_DIAL 35

uint8_t _addr = 0x5;
struct dial_t dial;

Screen screen;
Microchip_PAC193x PAC = Microchip_PAC193x(15000);

Button button[4] = {
	Button(BUTTON_CH0), Button(BUTTON_CH1), Button(BUTTON_MENU), Button(BUTTON_DIAL)
};

unsigned long cur_time;

struct channel_ctrl {
	float volt;
};

struct channel_ctrl ch_ctrl[2];


uint8_t read8(TwoWire *theWire, uint8_t reg) {
	theWire->beginTransmission(_addr);
	theWire->write(reg);
	theWire->endTransmission();

	theWire->requestFrom(_addr, (byte)1);
	return theWire->read();
}

void write8(TwoWire *theWire, uint8_t reg, uint8_t val) {
	theWire->beginTransmission(_addr);
	theWire->write(reg);
	theWire->write(val);
	theWire->endTransmission();
}

TwoWire I2CA = TwoWire(0);
TwoWire I2CB = TwoWire(1);

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
		volt = (uint16_t)(PAC.Voltage/10);
		ampere = (uint16_t)(PAC.Current/10);
		watt = (uint16_t)(PAC.Power*100);
		screen.pushPower(volt, ampere, watt, 0);
		updatePowerSense3();
		volt = (uint16_t)(PAC.Voltage/10);
		ampere = (uint16_t)(PAC.Current/10);
		watt = (uint16_t)(PAC.Power*100);
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
			screen.countDial(dial.cnt, cur_time);
			dial.cnt = 0;
		}
		screen.setTime(cur_time);
		vTaskDelay(100);
	}
}

void testVoltage(void)
{
	/*
	for (int i = 0; i < 0xF1; i++) {
		delay(100);
		write8(0x0, i);
	}
	*/
}

uint8_t reg2, reg3, reg4;
void loop() {

	//get_memory_info();
	//get_i2c_slaves();
	//write8(0x6, 0x0);
#if 1
	reg2 = read8(&I2CA, 0x2);
	reg3 = read8(&I2CA, 0x3);
	reg4 = read8(&I2CA, 0x4);

	Serial.println("=============");
	Serial.print("status: ");
	for (int i = 7; i >= 0; i--)
		Serial.print(bitRead(reg2, i));
	Serial.println();
	Serial.print("latch:  ");
	for (int i = 7; i >= 0; i--)
		Serial.print(bitRead(reg3, i));
	Serial.println();
	Serial.print("mask:   ");
	for (int i = 7; i >= 0; i--)
		Serial.print(bitRead(reg4, i));
	Serial.println("=============");
	//testVoltage();
#endif
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
