#include <ArduinoTrace.h>
#include <Wire.h>
#include "smartpower3.h"

uint32_t ctime1 = 0;

#define LED2	13
#define LED1	2

#define FAN		12

#define FREQ	5000
#define RESOLUTION	8

uint32_t fps_ch0;

void setup(void) {
	Serial.begin(115200);
	ARDUINOTRACE_INIT(115200);
	TRACE();
	I2CA.begin(15, 4, 200000);
	I2CB.begin(21, 22, 200000);
	I2CA.setClock(200000UL);
	I2CB.setClock(200000UL);
	PAC.begin(&I2CB);
	screen.begin(&I2CA);

	initEncoder(&dial);

	xTaskCreate(powerTask, "Read Power", 2000, NULL, 1, NULL);
	xTaskCreate(screenTask, "Draw Screen", 4000, NULL, 1, NULL);
	xTaskCreate(inputTask, "Input Task", 2000, NULL, 1, NULL);
	pinMode(25, INPUT_PULLUP);
	attachInterrupt(25, isr_stp, FALLING);

	ledcSetup(0, FREQ, RESOLUTION);
	ledcSetup(1, FREQ, RESOLUTION);
	ledcAttachPin(LED2, 0);
	ledcAttachPin(LED1, 1);

	ledcWrite(0, 50);
	ledcWrite(1, 50);
}

void isr_stp()
{
	screen.flag_int = 1;
}

void initPAC1933(void)
{
	PAC.UpdateProductID();
	PAC.UpdateManufacturerID();
	PAC.UpdateRevisionID();
}

void updatePowerSense3(void)
{
	PAC.update(2);
}

void updatePowerSense2(void)
{
	PAC.update(1);
}

void updatePowerSense1(void)
{
	PAC.update(0);
}

void powerTask(void *parameter)
{
	uint16_t volt, ampere, watt;
	int8_t *onoff;

	for (;;) {
		onoff = screen.getOnOff();
		if (onoff[0]) {
			updatePowerSense2();
			volt = (uint16_t)(PAC.Voltage);
			ampere = (uint16_t)(PAC.Current);
			watt = (uint16_t)(PAC.Power*1000);
			//Serial.printf("%d,%d,%d,%d\n\r", millis(), volt, ampere, watt);
			screen.pushPower(volt, ampere, watt, 0);
		}

		if (onoff[1]) {
			updatePowerSense3();
			volt = (uint16_t)(PAC.Voltage);
			ampere = (uint16_t)(PAC.Current);
			watt = (uint16_t)(PAC.Power*1000);
			screen.pushPower(volt, ampere, watt, 1);
			//Serial.printf("%d,%d,%d,%d\n\r", millis(), volt, ampere, watt);
		}

		if ((millis() - ctime1) > 500) {
			PAC.update(0);
			volt = (uint16_t)(PAC.Voltage);
			ampere = (uint16_t)(PAC.Current);
			watt = (uint16_t)(PAC.Power*100);
			screen.pushInputPower(volt, ampere, watt);
			ctime1 = millis();
		}
	}
}

void screenTask(void *parameter)
{
	for (;;) {
		screen.run();
		vTaskDelay(10);
	}
}

void inputTask(void *parameter)
{
	for (;;) {
		cur_time = millis();
		for (int i = 0; i < 4; i++) {
			if (button[i].checkPressed() == true) {
				//get_memory_info();
				screen.getBtnPress(i, cur_time);
			}
		}
		if (dial.cnt != 0) {
			screen.countDial(dial.cnt, dial.direct, cur_time);
			dial.cnt = 0;
		}
		screen.setTime(cur_time);
		vTaskDelay(10);
	}
}

void loop() {
	delay(500);
	//get_memory_info();
}

void get_memory_info(void)
{
	Serial.printf("Heap : %d, Free Heap : %d\n\r", ESP.getHeapSize(), ESP.getFreeHeap());
	Serial.printf("Stack High Water Mark %d\n\r", uxTaskGetStackHighWaterMark(NULL));
	Serial.printf("Psram : %d, Free Psram : %d\n\r", ESP.getPsramSize(), ESP.getFreePsram());
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
