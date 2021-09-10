#include <ArduinoTrace.h>
#include <Wire.h>
#include "smartpower3.h"
#include "wifi_manager.h"

uint32_t ctime1 = 0;

#define LED2	13
#define LED1	2

#define FAN		12

#define FREQ	5000
#define RESOLUTION	8

uint32_t fps_ch0;

void setup(void) {
	ARDUINOTRACE_INIT(500000);
	Serial.begin(500000);
	TRACE();
	I2CA.begin(15, 4, 10000);
	I2CB.begin(21, 22, 400000);
	PAC.begin(&I2CB);
	screen.begin(&I2CA);

	wifiManager = WifiManager().instance();

	initEncoder(&dial);

	xTaskCreate(screenTask, "Draw Screen", 2000, NULL, 1, NULL);
	xTaskCreate(inputTask, "Input Task", 1500, NULL, 10, NULL);
	xTaskCreate(logTask, "Log Task", 2000, NULL, 1, NULL);
	xTaskCreate(wifiTask, "WiFi Connection Task", 4000, NULL, 1, NULL);
	pinMode(25, INPUT_PULLUP);
	pinMode(26, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(25), isr_stpd01_ch0, FALLING);
	attachInterrupt(digitalPinToInterrupt(26), isr_stpd01_ch1, FALLING);

	ledcSetup(0, FREQ, RESOLUTION);
	ledcSetup(1, FREQ, RESOLUTION);
	ledcAttachPin(LED2, 0);
	ledcAttachPin(LED1, 1);

	ledcWrite(0, 50);
	ledcWrite(1, 50);
}

void isr_stpd01_ch0()
{
	screen.setIntFlag(0);
}

void isr_stpd01_ch1()
{
	screen.setIntFlag(1);
}

void initPAC1933(void)
{
	PAC.UpdateProductID();
	PAC.UpdateManufacturerID();
	PAC.UpdateRevisionID();
}

void logTask(void *parameter)
{
	char buffer_input[30];
	char buffer_ch0[26];
	char buffer_ch1[26];
	uint16_t log_interval;
	for (;;) {
		vTaskDelay(5);
		log_interval = screen.getLogInterval();
		if (log_interval > 0) {
			vTaskDelay(log_interval-5);
			sprintf(buffer_input, "%010d,%05d,%04d,%05d,%1d,", millis(), volt[0], amp[0], watt[0], low_input);
			sprintf(buffer_ch0, "%05d,%04d,%05d,%d,%x,", volt[1], amp[1], watt[1], onoff[0], 0xff);
			sprintf(buffer_ch1, "%05d,%04d,%05d,%d,%x\n\r", volt[2], amp[2], watt[2], onoff[1], 0xff);
			Serial.printf(buffer_input);
			Serial.printf(buffer_ch0);
			Serial.printf(buffer_ch1);
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

void wifiTask(void *parameter)
{
	// Run the WiFi service as a task in the background
	wifiManager.init();

	// Must not be terminated
	for (;;) {
		vTaskDelay(10000 / portTICK_PERIOD_MS);
	}
}

void loop() {
	PAC.Refresh();
	onoff = screen.getOnOff();
	if (onoff[0]) {
		PAC.update(1);
		volt[1] = (uint16_t)(PAC.Voltage);
		amp[1] = (uint16_t)(PAC.Current);
		watt[1] = (uint16_t)(PAC.Power*1000);
		screen.pushPower(volt[1], amp[1], watt[1], 0);
		wifiManager.setCurrentPower(
			CHANNEL_0,
			(WifiCurrentPower) {CHANNEL_0, true, volt[1], amp[1], watt[1]});
	} else {
		wifiManager.setCurrentPower(
			CHANNEL_0,
			(WifiCurrentPower) {CHANNEL_0, false, volt[1], amp[1], watt[1]});
	}

	if (onoff[1]) {
		PAC.update(2);
		volt[2] = (uint16_t)(PAC.Voltage);
		amp[2] = (uint16_t)(PAC.Current);
		watt[2] = (uint16_t)(PAC.Power*1000);
		screen.pushPower(volt[2], amp[2], watt[2], 1);
		wifiManager.setCurrentPower(
			CHANNEL_1,
			(WifiCurrentPower) {CHANNEL_1, true, volt[2], amp[2], watt[2]});
	} else {
		wifiManager.setCurrentPower(
			CHANNEL_1,
			(WifiCurrentPower) {CHANNEL_1, false, volt[2], amp[2], watt[2]});
	}

	if ((millis() - ctime1) > 500) {
		ctime1 = millis();
		PAC.update(0);
		volt[0] = (uint16_t)(PAC.Voltage);
		amp[0] = (uint16_t)(PAC.Current);
		watt[0] = (uint16_t)(PAC.Power*100);
		if (volt[0] < 6000) {
			screen.debug();
			PAC.update(0);
			volt[0] = (uint16_t)(PAC.Voltage);
			for (int i = 0; i < 3; i++) {
				if (volt[0] > 6000) {
					break;
				}
				low_input = true;
			}
		} else {
			low_input = false;
		}
		screen.pushInputPower(volt[0], amp[0], watt[0]);
	}
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
