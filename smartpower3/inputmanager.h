#include <ESP32Encoder.h>
#include <Arduino.h>
#include <FunctionalInterrupt.h>

#define DEBOUNCE_TIME 100

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
    volatile uint8_t pressed;
	volatile uint32_t debounceTimer;

public:
	Button(uint8_t pin);
	uint8_t checkPressed(void);
	void isr_pol(void);
};
