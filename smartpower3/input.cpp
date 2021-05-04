#include "input.h"

ESP32Encoder encoder;

struct dial_t {
	int8_t cnt;
	bool direct;
};

void initEncoder(void *dial)
{
  ESP32Encoder::useInternalWeakPullResistors = UP;
  encoder.attachHalfQuad(15, 4);
  encoder.setCount(0);
  xTaskCreate(countEncoder, "Task1", 1500, dial, 1, NULL);
}

void countEncoder(void *dial)
{
  struct dial_t *tmp = (struct dial_t *)dial;
  for (;;) {
    if (encoder.getCount() > 1) {
      encoder.setCount(0);
	  tmp->cnt += 1;
	  tmp->direct = true;
    } else if (encoder.getCount() < -1) {
      encoder.setCount(0);
	  tmp->cnt -= 1;
	  tmp->direct = false;
    }
    vTaskDelay(50);
  }
}
