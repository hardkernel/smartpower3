#include "inputmanager.h"

ESP32Encoder encoder;

Button::Button(uint8_t pin) : PIN(pin)
{
	pinMode(pin, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(pin), std::bind(&Button::isr, this), FALLING);
}

void Button::isr(void) {
	debounceTimer = millis();
	pressed = true;
}

void Button::attachInt(void)
{
	pinMode(PIN, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(PIN), std::bind(&Button::isr, this), FALLING);
}

bool Button::checkPressed(void) {
	if (pressed) {
		if (millis() - debounceTimer >= DEBOUNCE_TIME) {
			if (digitalRead(PIN) == 0) {
				pressed = false;
				return true;
			}
		}
	}
	return false;
}

void initEncoder(void *dial)
{
	ESP32Encoder::useInternalWeakPullResistors = UP;
	encoder.attachHalfQuad(33, 32);
	encoder.setCount(0);
	encoder.setFilter(500);
	xTaskCreate(countEncoder, "encoderTask", 1500, dial, 1, NULL);
}

void countEncoder(void *dial)
{
	struct dial_t *tmp = (struct dial_t *)dial;
	int8_t cnt;
	for (;;) {
		cnt = encoder.getCount();
		if (cnt > 5) {
			encoder.setCount(0);
			tmp->cnt += 1;
			tmp->direct = 1;
			tmp->step = 10;
		} else if (cnt < -5) {
			encoder.setCount(0);
			tmp->cnt -= 1;
			tmp->direct = -1;
			tmp->step = 10;
		} else if (cnt > 1) {
			encoder.setCount(0);
			tmp->cnt += 1;
			tmp->direct = 1;
			tmp->step = 1;
		} else if (cnt < -1) {
			encoder.setCount(0);
			tmp->cnt -= 1;
			tmp->direct = -1;
			tmp->step = 1;
		}
		vTaskDelay(100);
	}
}

