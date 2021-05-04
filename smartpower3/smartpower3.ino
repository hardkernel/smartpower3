#include "screen.h"
#include "input.h"

float val_volt = 3;
float val_ampere = 0;
float val_watt = 0;

struct dial_t {
	int8_t cnt;
	bool direct;
};

struct dial_t dial;

//int8_t dial;
uint8_t mode;
uint8_t pre_mode;
bool direct;

Screen screen;

#define BUTTON1 19
#define BUTTON2 32

Button button1(BUTTON1);
Button button2(BUTTON2);

void setup(void) {
	Serial.begin(115200);

	initEncoder(&dial);

	xTaskCreate(draw_power, "Draw power", 1500, NULL, 1, NULL);
}

void get_memory_info(void)
{
	Serial.printf("Heap : %d, Free Heap : %d\n", ESP.getHeapSize(), ESP.getFreeHeap());
	Serial.printf("Stack High Water Mark %d\n", uxTaskGetStackHighWaterMark(NULL));
	//  Serial.printf("Psram : %d, Free Psram : %d\n", ESP.getPsramSize(), ESP.getFreePsram());
}

#if 1
void draw_power(void *parameter)
{
	for (;;) {

		screen.drawVoltage(val_volt, 0);
		screen.drawAmpere(val_ampere, 0);
		screen.drawWatt(val_watt, 0);

		pre_mode = mode;
		mode = abs(dial.cnt%7);
		Serial.printf("%d, %d, %d\n", dial.cnt%7, dial.cnt, mode);

		switch (mode) {
			case 6:
				screen.channel[0]->clearOutLines(1);
				screen.header->drawOutLines();
			    screen.header->activate();
				screen.channel[1]->clearOutLines(1);
				break;
			case 5:
				screen.header->clearOutLines();
				screen.channel[0]->drawOutLines(1);
				screen.channel[0]->clearOutLines(2);
				break;
			case 4:
				screen.channel[0]->clearOutLines(1);
				screen.channel[0]->drawOutLines(2);
				screen.channel[0]->clearOutLines(3);
				break;
			case 3:
				screen.channel[0]->clearOutLines(2);
				screen.channel[0]->drawOutLines(3);
				screen.channel[1]->clearOutLines(3);
				break;
			case 2:
				screen.channel[0]->clearOutLines(3);
				screen.channel[1]->drawOutLines(3);
				screen.channel[1]->clearOutLines(2);
				break;
			case 1:
				screen.channel[1]->clearOutLines(3);
				screen.channel[1]->drawOutLines(2);
				screen.channel[1]->clearOutLines(1);
				break;
			case 0:
				screen.channel[1]->clearOutLines(2);
				screen.channel[1]->drawOutLines(1);
				screen.header->clearOutLines();
				break;
		}
		/*
		screen.channel[0]->drawOutLines(3);
		screen.channel[1]->drawOutLines(2);
		*/
		//get_memory_info();

		vTaskDelay(100);
	}
}
#endif

void loop() {
    button1.checkPressed();
    button2.checkPressed();
	if (screen.getMode() == 0) {
		if (dial.cnt != 0)
			screen.edit = 1;
	}
	delay(100);
}
