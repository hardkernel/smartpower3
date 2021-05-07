#include "screen.h"
#include "input.h"
#include "pac1933.h"
#include "Free_Fonts.h" // Include the header file attached to this sketch

#define BUTTON_0 33
#define BUTTON_1 32
#define BUTTON_MENU 39
#define BUTTON_DIAL 19

struct dial_t dial;

Screen screen;
PAC1933 pac1933;

Button button[4] = {
	Button(BUTTON_0), Button(BUTTON_1), Button(BUTTON_MENU), Button(BUTTON_DIAL)
};

bool check_btn[3] = {false,};

void setup(void) {
	Serial.begin(115200);

	initEncoder(&dial);

	xTaskCreate(readPower, "Read Power", 2000, NULL, 1, NULL);
	xTaskCreate(drawScreen, "Draw Screen", 2000, NULL, 1, NULL);
}

void get_memory_info(void)
{
	Serial.printf("Heap : %d, Free Heap : %d\n", ESP.getHeapSize(), ESP.getFreeHeap());
	Serial.printf("Stack High Water Mark %d\n", uxTaskGetStackHighWaterMark(NULL));
	//  Serial.printf("Psram : %d, Free Psram : %d\n", ESP.getPsramSize(), ESP.getFreePsram());
}

void drawScreen(void *parameter)
{
	for (;;) {
		screen.drawScreen(dial);
		vTaskDelay(100);
	}
}
void readPower(void *parameter)
{
	float volt, ampere, watt;
	for (;;) {
		volt = (float)pac1933.readVoltage()/10;
		ampere = (float)pac1933.readAmpere()/10;
		watt = (float)pac1933.readWatt()/100;
		screen.pushPower(volt, ampere, watt, 0);
		/*
		volt = (float)pac1933.readVoltage()/10;
		ampere = (float)pac1933.readAmpere()/10;
		watt = (float)pac1933.readWatt()/100;
		screen.pushPower(volt, ampere, watt, 1);
		*/
		vTaskDelay(500);
	}
}

void loop() {
	if (dial.cnt != 0) {
		screen.setModeCounter(dial.cnt);
		dial.cnt = 0;
		screen.setMode(BASE_MOVE);
	}
	for (int i = 0; i < 4; i++) {
		check_btn[i] = button[i].checkPressed();
	}
	if (check_btn[0] == true) {
		screen.powerOn(0);
	}
	if (check_btn[1] == true) {
		//screen.powerOn(1);
	}

	if (check_btn[2] == true) {
		if (screen.getMode() == BASE_MOVE) {
			screen.setMode(BASE_EDIT);
		}
	}

	delay(100);
}
