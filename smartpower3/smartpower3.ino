#include <Wire.h>
#include "smartpower3.h"

uint32_t ctime1 = 0;

void setup(void) {
	Serial.begin(115200);

	I2CA.begin(15, 4, 10000);
	I2CB.begin(21, 22, 400000);
	PAC.begin(&I2CB);
	screen.begin(&I2CA);

	initEncoder(&dial);

	xTaskCreate(screenTask, "Draw Screen", 3000, NULL, 1, NULL);
	xTaskCreate(inputTask, "Input Task", 1500, NULL, 10, NULL);
	xTaskCreate(logTask, "Log Task", 2000, NULL, 1, NULL);
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
			sprintf(buffer_input, "%010lu,%05d,%04d,%05d,%1d,", cur_time, volt[0], amp[0], watt[0], low_input);
			sprintf(buffer_ch0, "%05d,%04d,%05d,%d,%x,", volt[1], amp[1], watt[1], onoff[0], screen.getIntStat(0));
			sprintf(buffer_ch1, "%05d,%04d,%05d,%d,%x\n\r", volt[2], amp[2], watt[2], onoff[1], screen.getIntStat(1));
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
		for (int i = 0; i < 4; i++) {
			if (screen.checkAttachBtn(i))
				button[i].attachInt();
			if (button[i].checkPressed() == true)
				screen.getBtnPress(i, cur_time);
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
	PAC.Refresh();
	onoff = screen.getOnOff();
	for (int i = 1; i < 3; i++) {
		PAC.update(i);
		volt[i] = (uint16_t)(PAC.Voltage);
		amp[i] = (uint16_t)(PAC.Current);
		watt[i] = (uint16_t)(PAC.Power*1000);

#ifdef DEBUG_STPD01
		if (onoff[i-1]) {
			if (volt[i] < 3000)
				screen.countLowVoltage((uint8_t)(i-1));
		}
#endif
	}

	if ((millis() - ctime1) > 300) {
		ctime1 = millis();
		PAC.update(0);
		volt[0] = (uint16_t)(PAC.Voltage);
		amp[0] = (uint16_t)(PAC.Current);
		if (volt[0] < 6000) {
			screen.debug();
			PAC.Refresh();
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
		for (int i = 1; i < 3; i++)
			screen.pushPower(volt[i], amp[i], watt[i], i-1);

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
