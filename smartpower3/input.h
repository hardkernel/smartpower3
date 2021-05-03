#include <ESP32Encoder.h>
#include <Arduino.h>
#include <FunctionalInterrupt.h>

#define DEBOUNCE_TIME 250

void countEncoder(void *val);
void initEncoder(void *param);
void gpioIntrHandler0(void);
void gpioIntrHandler1(void);

/*
struct Button {
  uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
  volatile uint32_t debounceTimer;
};

  if (millis() - DEBOUNCE_TIME >= btn[1].debounceTimer) {
    if (digitalRead(btn[1].PIN) == 1) {
      btn[1].debounceTimer = millis();
      btn[1].numberKeyPresses += 1;
      btn[1].pressed = true;
	  Serial.println("hello");
    }
  }
*/

class Button
{
public:
	Button(uint8_t reqPin) : PIN(reqPin){
		pinMode(PIN, INPUT_PULLUP);
		//attachInterrupt(PIN, std::bind(&Button::isr,this), FALLING);
		attachInterrupt(PIN, std::bind(&Button::isr, this), FALLING);
	};
	~Button() {
		detachInterrupt(PIN);
	};

	void isr(void) {
		numberKeyPresses += 1;
		pressed = true;
	};

	void checkPressed() {
		if (pressed) {
			Serial.printf("Button on pin %u has been pressed %u times\n", PIN, numberKeyPresses);
			pressed = false;
		}
	};

    volatile bool pressed;
private:
	const uint8_t PIN;
    volatile uint32_t numberKeyPresses;
    volatile uint32_t debounceTimer;
};
