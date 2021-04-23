
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <ESP32Encoder.h>
#include "screen.h"

// Specify sprite 160 x 128 pixels (needs 40Kbytes of RAM for 16 bit colour)
#define IWIDTH  138
#define IHEIGHT 48
#define WIDTH_HEADER 240
#define HEIGHT_HEADER 40

// Pause in milliseconds between screens, change to 0 to time font rendering


unsigned long targetTime = 0; // Used for testing draw times

float val_volt = 3;
float val_ampere = 0;
float val_watt = 0;

String s_volt = "03.00";
String s_ampere = "00.00";
String s_watt = "00.00";

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
  volatile uint32_t debounceTimer;
};

#define DEBOUNCE_TIME 250

Button btn[3] = {{19, 0, false, 0}, {32, 0, false}, {33, 0, false}};

Screen screen;

void gpio_intr_handler0() {
  if (millis() - DEBOUNCE_TIME >= btn[0].debounceTimer) {
    if (digitalRead(btn[0].PIN) == 1) {
      btn[0].debounceTimer = millis();
      btn[0].numberKeyPresses += 1;
      btn[0].pressed = true;
    }
  }
}

void gpio_intr_handler1() {
  if (millis() - DEBOUNCE_TIME >= btn[1].debounceTimer) {
    if (digitalRead(btn[1].PIN) == 1) {
      btn[1].debounceTimer = millis();
      btn[1].numberKeyPresses += 1;
      btn[1].pressed = true;
    }
  }
}


ESP32Encoder encoder;

void setup(void) {
  Serial.begin(115200);
  init_encoder();

  xTaskCreate(draw_power, "Draw power", 1500, NULL, 1, NULL);

}

void init_encoder(void)
{
  pinMode(19, INPUT_PULLUP);
  attachInterrupt(19, gpio_intr_handler0, RISING);
  pinMode(32, INPUT_PULLUP);
  attachInterrupt(32, gpio_intr_handler1, RISING);
  
  ESP32Encoder::useInternalWeakPullResistors = UP;
  encoder.attachHalfQuad(4, 15);
  encoder.setCount(0);

  xTaskCreate(count_encoder, "Task1", 1500, NULL, 1, NULL);
}

void count_encoder(void *)
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

    vTaskDelay(100);
  }
}

uint8_t mode = 0;

void get_memory_info(void)
{
  Serial.printf("Heap : %d, Free Heap : %d\n", ESP.getHeapSize(), ESP.getFreeHeap());
  Serial.printf("Stack High Water Mark %d\n", uxTaskGetStackHighWaterMark(NULL));
//  Serial.printf("Psram : %d, Free Psram : %d\n", ESP.getPsramSize(), ESP.getFreePsram());
}

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

    }

    screen.drawVoltage(val_volt);
    screen.drawAmpere(val_ampere);
    screen.drawWatt(val_watt);
    vTaskDelay(100);
    
  }
}

void loop() {
  targetTime = millis();

  #if 0

  //  val_ampere = val_ampere + 0.02;
  if (val_ampere > 99 or val_ampere < 0)
    val_ampere = 0;

  //  val_watt = val_watt + 0.03;
  if (val_watt > 99 or val_watt < 0)
    val_watt = 0;

  if (mode == 1) {
    tft.drawRect(10, 50, 200, 250, TFT_YELLOW);
    mode = 0;
  }
  #endif

  delay(10);
}
