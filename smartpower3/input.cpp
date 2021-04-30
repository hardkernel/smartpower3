#include "input.h"

Input::Input(void)
{
	initEncoder();
}

void Input::initEncoder(void)
{
  ESP32Encoder::useInternalWeakPullResistors = UP;
  encoder.attachHalfQuad(15, 4);
  encoder.setCount(0);

  xTaskCreate(countEncoder, "Task1", 1500, NULL, 1, NULL);
}

#if 0
void Input::gpioIntrHandler0() {
  if (millis() - DEBOUNCE_TIME >= btn[0].debounceTimer) {
    if (digitalRead(btn[0].PIN) == 1) {
      btn[0].debounceTimer = millis();
      btn[0].numberKeyPresses += 1;
      btn[0].pressed = true;
    }
  }
}

void Input::gpioIntrHandler1() {
  if (millis() - DEBOUNCE_TIME >= btn[1].debounceTimer) {
    if (digitalRead(btn[1].PIN) == 1) {
      btn[1].debounceTimer = millis();
      btn[1].numberKeyPresses += 1;
      btn[1].pressed = true;
    }
  }
}
#endif

void Input::countEncoder(void *)
{
  for (;;) {
    if (encoder.getCount() > 1) {
      encoder.setCount(0);
      val_volt = val_volt + 0.1;
      if (val_volt > 99 or val_volt < 3)
        val_volt = 3;
    } else if (encoder.getCount() < -1) {
      encoder.setCount(0);
      val_volt = val_volt - 0.1;
      if (val_volt > 99 or val_volt < 3)
        val_volt = 3;
    }

    vTaskDelay(50);
  }
}
