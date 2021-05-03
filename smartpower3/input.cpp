#include "input.h"

ESP32Encoder encoder;

void initEncoder(void *dial)
{
  ESP32Encoder::useInternalWeakPullResistors = UP;
  encoder.attachHalfQuad(15, 4);
  encoder.setCount(0);
  xTaskCreate(countEncoder, "Task1", 1500, dial, 1, NULL);
}

void gpioIntrHandler0() {
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

void gpioIntrHandler1() {
/*
  if (millis() - DEBOUNCE_TIME >= btn[1].debounceTimer) {
    if (digitalRead(btn[1].PIN) == 1) {
      btn[1].debounceTimer = millis();
      btn[1].numberKeyPresses += 1;
      btn[1].pressed = true;
	  Serial.println("hello");
    }
  }
*/
}


void countEncoder(void *dial)
{
  for (;;) {
    if (encoder.getCount() > 1) {
      encoder.setCount(0);
	  Serial.println("UP");
	  *(int8_t *)dial += 1;
    } else if (encoder.getCount() < -1) {
      encoder.setCount(0);
	  Serial.println("DOWN");
	  *(int8_t *)dial -= 1;
    }
	//Serial.printf("%d, %d\n", btn[0].pressed, btn[1].pressed);

    vTaskDelay(50);
  }
}
