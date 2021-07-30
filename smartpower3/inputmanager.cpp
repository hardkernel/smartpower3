#include "inputmanager.h"

ESP32Encoder encoder;

Button::Button(uint8_t pin) : PIN(pin)
{
	pinMode(pin, INPUT_PULLUP);
	attachInterrupt(pin, std::bind(&Button::isr, this), RISING);
}

void Button::isr(void) {
	if (millis() - debounceTimer >= DEBOUNCE_TIME) {
		if (digitalRead(PIN) == 1) {
			debounceTimer = millis();
			numberKeyPresses += 1;
			pressed = true;
		}
	}
}

bool Button::checkPressed(void) {
	if (pressed) {
		pressed = false;
		return true;
	}
	return false;
}

void initEncoder(void *dial)
{
  ESP32Encoder::useInternalWeakPullResistors = UP;
  encoder.attachHalfQuad(33, 32);
  encoder.setCount(0);
  encoder.setFilter(500);
  xTaskCreate(countEncoder, "Task1", 1500, dial, 1, NULL);
}

void countEncoder(void *dial)
{
  struct dial_t *tmp = (struct dial_t *)dial;
  int8_t cnt;
  for (;;) {
	cnt = encoder.getCount();
    //if (cnt > 1) {
    if (cnt > 0) {
      encoder.setCount(0);
	  tmp->cnt += 1;
	  tmp->direct = true;
    } else if (cnt < 0) {
    //} else if (cnt < -1) {
      encoder.setCount(0);
	  tmp->cnt -= 1;
	  tmp->direct = false;
    }
    vTaskDelay(100);
  }
}

