#include <ArduinoTrace.h>
#include <Wire.h>
#include "smartpower3.h"

uint32_t ctime1 = 0;
uint32_t ctime3 = 0;

uint16_t vmax3, amax3, wmax3;
uint16_t vmin3 = 99999;
uint16_t amin3 = 99999;
uint16_t wmin3 = 99999;

#define LED2	13
#define LED1	2

#define FAN		12

#define FREQ	5000
#define RESOLUTION	8

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

	ble = BleManager().instance();
	ble.init();

	// TODO: Do not start the BLE advertizing by default
	ble.start();

	initEncoder(&dial);

	// - If the total allocated stack size is over 9000,
	// an error occurs when the BLE is about to be connected
	// - Error occurs without pinning each task to a specific CPU
	xTaskCreatePinnedToCore(
		powerTask, "Read Power",
		2000, // Error occurs if the number is less than 2000
		NULL, 1, NULL,
		APP_CPU_NUM); // Error occurs if it allocated to PRO_CPU
	xTaskCreatePinnedToCore(
		screenTask, "Draw Screen",
		3000, // Error occurs if the number is less than 3000
		NULL, 1, NULL,
		PRO_CPU_NUM);
	xTaskCreatePinnedToCore(
		inputTask, "Input Task",
		1000, // Error occurs if the number is less than 1000
		NULL, 1, NULL,
		PRO_CPU_NUM);
	xTaskCreatePinnedToCore(
		bleTask, "BLE Advertise",
		3000, // Error occurs if the number is less than 3000
		NULL, 1, NULL,
		APP_CPU_NUM);

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
	//Serial.println("hello isr");
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

	// Error occurs without this delay
	vTaskDelay(3000);

	for (;;) {
		updatePowerSense2();
		volt = (uint16_t)(PAC.Voltage);
		ampere = (uint16_t)(PAC.Current);
		watt = (uint16_t)(PAC.Power*1000);
		screen.pushPower(volt, ampere, watt, 0);
		ble.setCurrentPowerInfo((BleDevicePowerInfo) {
			CHANNEL_0, volt, ampere, watt
		});

		if (volt > vmax3)
			vmax3 = volt;
		if (ampere > amax3)
			amax3 = ampere;
		if (volt < vmin3)
			vmin3 = volt;
		if (ampere < amin3)
			amin3 = ampere;

		/*
		if ((millis() - ctime3) > 3000) {
			Serial.printf("================== CH0_PAC2 =============\n\r");
			Serial.printf("vmax : [[ %d ]], vmin : [[ %d ]], vcur : [[ %d ]], vdiff : [[ %d ]] \n\r", vmax3, vmin3, volt, vmax3 - vmin3);
			Serial.printf("amax : [[ %d ]], amin : [[ %d ]], acur : [[ %d ]], adiff : [[[[ %d ]]]] \n\r", amax3, amin3, ampere, amax3 - amin3);
			Serial.printf("===============================\n\r");
			ctime3 = millis();
		}
		*/

		updatePowerSense3();
		volt = (uint16_t)(PAC.Voltage);
		ampere = (uint16_t)(PAC.Current);
		watt = (uint16_t)(PAC.Power*1000);
		screen.pushPower(volt, ampere, watt, 1);
		ble.setCurrentPowerInfo((BleDevicePowerInfo) {
			CHANNEL_1, volt, ampere, watt
		});

		if ((millis() - ctime1) > 500) {
			updatePowerSense1();
			volt = (uint16_t)(PAC.Voltage);
			ampere = (uint16_t)(PAC.Current);
			watt = (uint16_t)(PAC.Power*100);
			screen.pushInputPower(volt, ampere, watt);
			ctime1 = millis();
		}

		vTaskDelay(10);
	}
}

void screenTask(void *parameter)
{
	// Error occurs without this delay
	vTaskDelay(3000);

	for (;;) {
		screen.run();
		vTaskDelay(10);
	}
}

void inputTask(void *parameter)
{
	// Error occurs without this delay
	vTaskDelay(3000);

	for (;;) {
		cur_time = millis();
		for (int i = 0; i < 4; i++) {
			if (button[i].checkPressed() == true) {
				//get_memory_info();
				screen.getBtnPress(i, cur_time);
				if (i == 2) {
					vmax3 = 0;
					amax3 = 0;
					wmax3 = 0;
					vmin3 = 99999;
					amin3 = 99999;
					wmin3 = 99999;
				}
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

void bleTask(void *parameter)
{
	uint16_t chIndex;

	// Error occurs without this delay
	vTaskDelay(3000);
	ble.serialLogLine("The notifying task started");

	for (;;) {
		if (ble.getBleServiceState() == BLE_SERVICE_ON &&
		    ble.getConnectionState() == BLE_DEVICE_CONNECTED) {
			for (chIndex = 0; chIndex < MAX_CHANNEL_NUM; chIndex++) {
				ble.notify(chIndex);
			}

			// Should not type a number less than 10
			vTaskDelay(10);
		} else {
			vTaskDelay(1000);
		}
	}
}

void updatePowerSense1_PAC2(void)
{
	/*
	PAC2.UpdateVoltageSense1();
	PAC2.UpdateCurrentSense1();
	PAC2.UpdatePowerSense1();
	*/
}


void testI2CA()
{
	screen.run();
#if 0
	uint16_t volt, ampere, watt;
	updatePowerSense1_PAC2();
	volt = (uint16_t)(PAC2.Voltage);
	ampere = (uint16_t)(PAC2.Current);
	watt = (uint16_t)(PAC2.Power*100);

	if (volt > vmax3)
		vmax3 = volt;
	if (ampere > amax3)
		amax3 = ampere;
	if (volt < vmin3)
		vmin3 = volt;
	if (ampere < amin3)
		amin3 = ampere;

	if ((millis() - ctime3) > 3000) {
		Serial.printf("================== CH0_PAC2 =============\n\r");
		Serial.printf("vmax : [[ %d ]], vmin : [[ %d ]], vcur : [[ %d ]], vdiff : [[ %d ]] \n\r", vmax3, vmin3, volt, vmax3 - vmin3);
		Serial.printf("amax : [[ %d ]], amin : [[ %d ]], acur : [[ %d ]], adiff : [[[[ %d ]]]] \n\r", amax3, amin3, ampere, amax3 - amin3);
		Serial.printf("===============================\n\r");
		ctime3 = millis();
	}
#endif
}

int int_old = -1;
void loop() {
	//get_i2c_slaves(&I2CB);
	//get_i2c_slaves(&I2CA);
	/*
	ledcWrite(0, 0);
	delay(500);
	ledcWrite(0, 50);
	delay(500);
	*/
	delay(1000);
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
