#include "smartpower3.h"
#include <Wire.h>

uint16_t vmax = 0;
uint16_t amax = 0;
uint16_t vmin = -1;
uint16_t amin = -1;

uint16_t vmax2 = 0;
uint16_t amax2 = 0;
uint16_t vmin2 = -1;
uint16_t amin2 = -1;

uint32_t ctime0 = 0;

void setup(void) {
	pinMode(STPD01_CH0, OUTPUT);
	pinMode(STPD01_CH1, OUTPUT);
	digitalWrite(STPD01_CH0, LOW);
	digitalWrite(STPD01_CH1, LOW);

	Serial.begin(115200);
	I2CA.begin(15, 4);
	I2CB.begin(21, 22);
	PAC.begin(&I2CB);

	screen.begin(&I2CA);

	initEncoder(&dial);
	//initPAC1933();

	xTaskCreate(powerTask, "Read Power", 2000, NULL, 1, NULL);
	xTaskCreate(screenTask, "Draw Screen", 2000, NULL, 1, NULL);
	xTaskCreate(inputTask, "Input Task", 2000, NULL, 1, NULL);
}

void initPAC1933(void)
{
	PAC.UpdateProductID();
	PAC.UpdateManufacturerID();
	PAC.UpdateRevisionID();
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
		watt = (uint16_t)(PAC.Power*100);
		screen.pushPower(volt, ampere, watt, 0);

		if (volt > vmax)
			vmax = volt;
		if (ampere > amax)
			amax = ampere;
		if (volt < vmin)
			vmin = volt;
		if (ampere < amin)
			amin = ampere;

		updatePowerSense3();
		volt = (uint16_t)(PAC.Voltage);
		ampere = (uint16_t)(PAC.Current);
		watt = (uint16_t)(PAC.Power*100);
		screen.pushPower(volt, ampere, watt, 1);
		if (volt > vmax2)
			vmax2 = volt;
		if (ampere > amax2)
			amax2 = ampere;
		if (volt < vmin2)
			vmin2 = volt;
		if (ampere < amin2)
			amin2 = ampere;

		if ((millis() - ctime0) > 3000) {
			Serial.printf("================== CH0 =============\n\r");
			Serial.printf("vmax : [[ %d ]], vmin : [[ %d ]], vcur : [[ %d ]], vdiff : [[ %d ]] \n\r", vmax, vmin, volt, vmax - vmin);
			Serial.printf("amax : [[ %d ]], amin : [[ %d ]], acur : [[ %d ]], adiff : [[[[ %d ]]]] \n\r", amax, amin, ampere, amax - amin);
			Serial.printf("===============================\n\r");
			Serial.printf("================== CH1 =============\n\r");
			Serial.printf("vmax : [[ %d ]], vmin : [[ %d ]], vcur : [[ %d ]], vdiff : [[ %d ]] \n\r", vmax2, vmin2, volt, vmax2 - vmin2);
			Serial.printf("amax : [[ %d ]], amin : [[ %d ]], acur : [[ %d ]], adiff : [[[[ %d ]]]] \n\r", amax2, amin2, ampere, amax2 - amin2);
			Serial.printf("===============================\n\r");
			ctime0 = millis();
		}

		updatePowerSense1();
		volt = (uint16_t)(PAC.Voltage);
		ampere = (uint16_t)(PAC.Current);
		watt = (uint16_t)(PAC.Power*100);
		screen.pushInputPower(volt, ampere, watt);

		vTaskDelay(10);
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
				screen.getBtnPress(i, cur_time);
				if (i == 2) {
					vmax = 0;
					amax = 0;
					vmin = -1;
					amin = -1;
					vmax2 = 0;
					amax2 = 0;
					vmin2 = -1;
					amin2 = -1;
				}
				/*
				if (i < 2)
					stpd01[i]->onOff();
				*/
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
	//get_memory_info();
	//get_i2c_slaves(&I2CA);
	//Serial.println(digitalRead(25));
	delay(1000);
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

