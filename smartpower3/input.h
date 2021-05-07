#include <ESP32Encoder.h>
#include <Arduino.h>
#include <FunctionalInterrupt.h>

#define DEBOUNCE_TIME 250

void countEncoder(void *val);
void initEncoder(void *param);

class Button
{
public:
	Button(uint8_t reqPin) : PIN(reqPin){
		pinMode(PIN, INPUT_PULLUP);
		attachInterrupt(PIN, std::bind(&Button::isr, this), RISING);
	};
	~Button() {
		detachInterrupt(PIN);
	}

	void IRAM_ATTR isr(void) {
		if (millis() - debounceTimer >= DEBOUNCE_TIME) {
			if (digitalRead(PIN) == 1) {
				debounceTimer = millis();
				numberKeyPresses += 1;
				pressed = true;
			}
		}
	}

	bool checkPressed(void) {
		if (pressed) {
			pressed = false;
			return true;
		}
		return false;
	}

private:
	const uint8_t PIN;
    volatile uint8_t numberKeyPresses;
    volatile bool pressed;
	volatile uint32_t debounceTimer;
};
