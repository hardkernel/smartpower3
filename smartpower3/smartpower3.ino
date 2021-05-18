#include "screen.h"
#include "inputmanager.h"
#include "pac1933.h"

#define BUTTON_MENU 36
#define BUTTON_CH0 39
#define BUTTON_CH1 34
#define BUTTON_DIAL 35

struct dial_t dial;

Screen screen;
PAC1933 pac1933;

Button button[4] = {
	Button(BUTTON_CH0), Button(BUTTON_CH1), Button(BUTTON_MENU), Button(BUTTON_DIAL)
};

unsigned long cur_time;

struct channel_ctrl {
	float volt;
};

struct channel_ctrl ch_ctrl[2];

void setup(void) {
	Serial.begin(115200);

	initEncoder(&dial);
	xTaskCreate(powerTask, "Read Power", 2000, NULL, 1, NULL);
	xTaskCreate(screenTask, "Draw Screen", 2000, NULL, 1, NULL);
	xTaskCreate(inputTask, "Input Task", 2000, NULL, 1, NULL);
	pinMode(14, OUTPUT);
	pinMode(27, OUTPUT);
	digitalWrite(14, HIGH);
	digitalWrite(27, HIGH);
}

void loop() {
	delay(100);
	//Serial.println(ESP.getFlashChipSize());
}

void powerTask(void *parameter)
{
	float volt, ampere, watt;
	for (;;) {
		volt = (float)pac1933.readVoltage()/10;
		ampere = (float)pac1933.readAmpere()/10;
		watt = (float)pac1933.readWatt()/100;
		if (!screen.getOnOff(0))
			screen.pushPower(0, 0, 0, 0);
		else
			screen.pushPower(volt, ampere, watt, 0);
		/*
		volt = (float)pac1933.readVoltage()/10;
		ampere = (float)pac1933.readAmpere()/10;
		watt = (float)pac1933.readWatt()/100;
		screen.pushPower(volt, ampere, watt, 1);
		*/
		vTaskDelay(200);
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

void get_memory_info(void)
{
	Serial.printf("Heap : %d, Free Heap : %d\n", ESP.getHeapSize(), ESP.getFreeHeap());
	Serial.printf("Stack High Water Mark %d\n", uxTaskGetStackHighWaterMark(NULL));
	Serial.printf("Psram : %d, Free Psram : %d\n", ESP.getPsramSize(), ESP.getFreePsram());
}
