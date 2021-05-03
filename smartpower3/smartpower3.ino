#include "screen.h"
#include "input.h"

float val_volt = 3;
float val_ampere = 0;
float val_watt = 0;

int8_t dial;

Screen screen;

#define BUTTON1 19
#define BUTTON2 32

Button button1(BUTTON1);
//Button button2(BUTTON2);

void gpioIntrHandler() {
Serial.println("HJODJFOJSDFI");
/*
  if (millis() - DEBOUNCE_TIME >= btn[0].debounceTimer) {
    if (digitalRead(btn[0].PIN) == 1) {
      btn[0].debounceTimer = millis();
      btn[0].numberKeyPresses += 1;
      btn[0].pressed = true;
	  Serial.println("hello");
    }
  }
*/
}

void setup(void) {
	Serial.begin(115200);

	pinMode(19, INPUT_PULLUP);
	pinMode(32, INPUT_PULLUP);
	//attachInterrupt(digitalPinToInterrupt(19), btn2.gpioIntrHandler, RISING);
	//attachInterrupt(digitalPinToInterrupt(32), gpioIntrHandler1, RISING);
	attachInterrupt(digitalPinToInterrupt(32), gpioIntrHandler, RISING);
	initEncoder(&dial);

	xTaskCreate(draw_power, "Draw power", 1500, NULL, 1, NULL);
}

uint8_t mode = 0;

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
		//Serial.printf("dr %d, %d\n", btn[0].pressed, btn[1].pressed);
		/*
		if (btn[0].pressed) {
			Serial.printf("Button 0 has been pressed %u times\n", btn[0].numberKeyPresses);
			btn[0].pressed = false;
			//      screen.tft_header.deleteSprite();
		}
		if (btn[1].pressed) {
			Serial.printf("Button 1 has been pressed %u times\n", btn[1].numberKeyPresses);
			btn[1].pressed = false;
			//      screen.tft_volt.deleteSprite();
			mode = 1;
			screen.getMode();
		}
		*/

		screen.drawVoltage(val_volt, 0);
		screen.drawAmpere(val_ampere, 0);
		screen.drawWatt(val_watt, 0);
		vTaskDelay(100);
		//get_memory_info();

	}
}
#endif

void loop() {
    button1.checkPressed();
    //button2.checkPressed();
	if (screen.getMode() == 0) {
		if (dial != 0)
			screen.edit = 1;
	}
    Serial.printf("HELLO %d", button1.pressed);
	delay(100);
}
