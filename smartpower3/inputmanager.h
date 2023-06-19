#include <ESP32Encoder.h>
#include <Arduino.h>
//#include <FunctionalInterrupt.h>

#define DEBOUNCE_TIME 100
#define LONG_PRESS_TIME 3000

void countEncoder(void *val);
void initEncoder(void *param);


struct dial_t {
	int8_t cnt;
	int8_t direct;
	uint8_t step = 1;
};


class Button
{
private:
	const uint8_t PIN;
    volatile uint8_t pressed = 0;
	volatile uint32_t pressed_time = 0;
public:
	Button(uint8_t pin);
	uint8_t checkPressed(void);
	void isr_pol(void);
};
