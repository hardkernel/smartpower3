#include "inputmanager.h"

ESP32Encoder encoder;

Button::Button(uint8_t pin) : PIN(pin)
{
	pinMode(pin, INPUT_PULLUP);
}

void Button::isr_pol(void)
{
	if (pressed == 0 && digitalRead(PIN) == 0) {
		pressed_time = millis();
		pressed = 1;
	} else if (digitalRead(PIN) == 1 && pressed == 2) {
		pressed = 0;
	}
}

uint8_t Button::checkPressed(void)
{
	if (pressed == 1 && (millis() - pressed_time >= DEBOUNCE_TIME)) {
		if (millis() - pressed_time >= LONG_PRESS_TIME) {
			pressed = 2;
			return 2;
		} else if (digitalRead(PIN) == 1) {
			pressed = 2;
			return 1;
		}
	}
	return 0;
}


void initEncoder(void *dial)
{
	ESP32Encoder::useInternalWeakPullResistors = UP;
	encoder.attachFullQuad(33, 32);
	encoder.setCount(0);
	encoder.setFilter(1023);
	xTaskCreate(countEncoder, "encoderTask", 672, dial, 1, NULL);
}

void countEncoder(void *dial)
{
	struct dial_t *tmp = (struct dial_t *)dial;
	int8_t cnt;

	for (;;) {
		cnt = encoder.getCount();
		if (cnt > 9) {
			encoder.setCount(0);
			tmp->cnt += 1;
			tmp->direct = 1;
			tmp->step = 3;
			vTaskDelay(5);
			continue;
		} else if (cnt < -9) {
			encoder.setCount(0);
			tmp->cnt -= 1;
			tmp->direct = -1;
			tmp->step = 3;
			vTaskDelay(5);
			continue;
		} else if (cnt > 1) {
			encoder.setCount(0);
			tmp->cnt += 1;
			tmp->direct = 1;
			tmp->step = 1;
			vTaskDelay(55);
			continue;
		} else if (cnt < -1) {
			encoder.setCount(0);
			tmp->cnt -= 1;
			tmp->direct = -1;
			tmp->step = 1;
			vTaskDelay(55);
			continue;
		}
		vTaskDelay(100);
	}
}

