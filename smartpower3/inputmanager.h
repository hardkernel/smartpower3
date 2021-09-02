#include <ESP32Encoder.h>
#include <Arduino.h>
#include <FunctionalInterrupt.h>

#define DEBOUNCE_TIME 100

void countEncoder(void *val);
void initEncoder(void *param);

struct dial_t {
	int8_t cnt;
	bool direct;
	uint8_t step;
};

class Button
{
private:
	const uint8_t PIN;
    volatile uint8_t numberKeyPresses;
    volatile bool pressed;
	volatile uint32_t debounceTimer;

public:
	Button(uint8_t pin);
	~Button() {
		detachInterrupt(PIN);
	}
	void IRAM_ATTR isr(void);
	bool checkPressed(void);
};
