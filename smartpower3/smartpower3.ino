#include "screen.h"
#include "input.h"

#define DEBOUNCE_TIME 250

float val_volt = 3;
float val_ampere = 0;
float val_watt = 0;

Screen screen;
Input *input = new Input();

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
  volatile uint32_t debounceTimer;
};

static Button btn[3] = {{19, 0, false, 0}, {32, 0, false}, {33, 0, false}};
	
void gpioIntrHandler0() {
  if (millis() - DEBOUNCE_TIME >= btn[0].debounceTimer) {
    if (digitalRead(btn[0].PIN) == 1) {
      btn[0].debounceTimer = millis();
      btn[0].numberKeyPresses += 1;
      btn[0].pressed = true;
    }
  }
}

void setup(void) {
  Serial.begin(115200);

  pinMode(19, INPUT_PULLUP);
  //pinMode(32, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(19), gpioIntrHandler0, RISING);
  //attachInterrupt(digitalPinToInterrupt(32), gpioIntrHandler1, RISING);

  //xTaskCreate(draw_power, "Draw power", 1500, NULL, 1, NULL);
}

uint8_t mode = 0;

void get_memory_info(void)
{
  Serial.printf("Heap : %d, Free Heap : %d\n", ESP.getHeapSize(), ESP.getFreeHeap());
  Serial.printf("Stack High Water Mark %d\n", uxTaskGetStackHighWaterMark(NULL));
//  Serial.printf("Psram : %d, Free Psram : %d\n", ESP.getPsramSize(), ESP.getFreePsram());
}

#if 0
void draw_power(void *parameter)
{
  for (;;) {
    if (btn[0].pressed) {
      Serial.printf("Button 0 has been pressed %u times\n", btn[0].numberKeyPresses);
      btn[0].pressed = false;
//      screen.tft_header.deleteSprite();
    }
    if (btn[1].pressed) {
      Serial.printf("Button 1 has been pressed %u times\n", btn[1].numberKeyPresses);
      btn[1].pressed = false;
//      screen.tft_volt.deleteSprite();
      mode = 1;
      screen.getMode();

    }

    screen.drawVoltage(val_volt, 0);
    screen.drawAmpere(val_ampere, 0);
    screen.drawWatt(val_watt, 0);
    vTaskDelay(50);
    get_memory_info();
    
  }
}
#endif

void loop() {
  #if 0

  //  val_ampere = val_ampere + 0.02;
  if (val_ampere > 99 or val_ampere < 0)
    val_ampere = 0;

  //  val_watt = val_watt + 0.03;
  if (val_watt > 99 or val_watt < 0)
    val_watt = 0;

  if (mode == 1) {
    mode = 0;
  }
  #endif

  delay(10);
}
