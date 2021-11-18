#include <Wire.h>
#include "smartpower3.h"

uint32_t ctime1 = 0;

void setup(void) {
	Serial.begin(115200);

	NVS.begin();

	I2CA.begin(15, 4, 10000);
	I2CB.begin(21, 22, 800000);
	PAC.begin(&I2CB);
	screen.begin(&I2CA);

	initEncoder(&dial);

	xTaskCreate(screenTask, "Draw Screen", 6000, NULL, 1, NULL);
	xTaskCreate(inputTask, "Input Task", 3000, NULL, 10, NULL);
	xTaskCreate(logTask, "Log Task", 4000, NULL, 1, NULL);
	xTaskCreate(btnTask, "Button Task", 4000, NULL, 1, NULL);
	pinMode(25, INPUT_PULLUP);
	pinMode(26, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(25), isr_stpd01_ch0, FALLING);
	attachInterrupt(digitalPinToInterrupt(26), isr_stpd01_ch1, FALLING);
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

void btnTask(void *parameter)
{
	for (;;) {
		for (int i = 0; i < 4; i++)
			button[i].isr_pol();
		vTaskDelay(10);
	}
}

void logTask(void *parameter)
{
	char buffer_input[30];
	char buffer_ch0[26];
	char buffer_ch1[26];
	uint16_t log_interval;
	for (;;) {
		cur_time = millis();
		vTaskDelay(5);
		log_interval = screen.getLogInterval();
		if (log_interval > 0) {
			vTaskDelay(log_interval-5);
			sprintf(buffer_input, "%010lu,%05d,%04d,%05d,%1d,", cur_time, mCh0.V(), mCh0.A(log_interval), mCh0.W(log_interval), low_input);
			sprintf(buffer_ch0, "%05d,%04d,%05d,%d,%x,", mCh1.V(), mCh1.A(log_interval), mCh1.W(log_interval), onoff[0], screen.getIntStat(0));
			sprintf(buffer_ch1, "%05d,%04d,%05d,%d,%x\n\r", mCh2.V(), mCh2.A(log_interval), mCh2.W(log_interval),onoff[1], screen.getIntStat(1));
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
	uint8_t pressed;
	for (;;) {
		for (int i = 0; i < 4; i++) {
			pressed = button[i].checkPressed();
			if (pressed == 1)
				screen.getBtnPress(i, cur_time);
			else if (pressed == 2)
				screen.getBtnPress(i, cur_time, true);
		}
		if (dial.cnt != 0) {
			screen.countDial(dial.cnt, dial.direct, dial.step, cur_time);
			dial.cnt = 0;
		}
		screen.setTime(cur_time);
		vTaskDelay(10);
	}
}

void loop() {
	onoff = screen.getOnOff();

	mChs.sample();

	if ((millis() - ctime1) > 300) {
		ctime1 = millis();
		if (mCh0.V() < 6000) {
			screen.debug();
			for (int i = 0; i < 3; i++) {
				mChs.sample();
				if (mCh0.V() > 6000) {
					break;
				}
				low_input = true;
			}
		} else {
			low_input = false;
		}
		screen.pushInputPower(mCh0.V(), mCh0.A(300), mCh0.W(300));
		screen.pushPower(mCh1.V(), mCh1.A(300), mCh1.W(300), 0);
		screen.pushPower(mCh2.V(), mCh2.A(300), mCh2.W(300), 1);

		if (!screen.getShutdown()) {
			if ((cur_time/1000)%2)
				screen.writeSysLED(50);
			else
				screen.writeSysLED(0);
			screen.writePowerLED(50);
		}
	}

	if (screen.getShutdown()) {
		screen.dimmingLED(1);
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
