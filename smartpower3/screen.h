#include <Arduino.h>
#include <TFT_eSPI.h>
#include "channel.h"

class Screen
{
private:
	uint8_t mode = 0;
	TFT_eSPI tft = TFT_eSPI();
	void initBaseMode(void);
	float cnt = 0;

public:
	bool edit = 0;
	Screen();
	void setMode(uint8_t val);
	uint8_t getMode(void);
	void drawHeader(String s);
	void drawScreen(void);
	void drawVoltage(float volt, uint8_t ch);
	void drawAmpere(float ampere, uint8_t ch);
	void drawWatt(float watt, uint8_t ch);
	void changeMode(uint8_t val);
	Component *header;
	Channel *channel[2];
	void activated(uint8_t idx);
};
