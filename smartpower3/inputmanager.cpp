#include "inputmanager.h"

ESP32Encoder encoder;

void initEncoder(void *dial)
{
  ESP32Encoder::useInternalWeakPullResistors = UP;
  encoder.attachHalfQuad(33, 32);
  encoder.setCount(0);
  xTaskCreate(countEncoder, "Task1", 1500, dial, 1, NULL);
}

void countEncoder(void *dial)
{
  struct dial_t *tmp = (struct dial_t *)dial;
  int8_t cnt;
  for (;;) {
	cnt = encoder.getCount();
    if (cnt > 1) {
      encoder.setCount(0);
	  tmp->cnt += 1;
	  tmp->direct = true;
    } else if (cnt < -1) {
      encoder.setCount(0);
	  tmp->cnt -= 1;
	  tmp->direct = false;
    }
    vTaskDelay(50);
  }
}

