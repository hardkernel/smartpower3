#include <Arduino.h>
#include <TFT_eSPI.h>
#include "channel.h"

enum screen_mode_t {
	BASE,
	BASE_MOVE,
	BASE_EDIT,
	SETTING
};

struct dial_t {
	int8_t cnt;
	bool direct;
};

class Screen
{
private:
	TFT_eSPI tft = TFT_eSPI();
	screen_mode_t mode = BASE;
	Component *header;
	Channel *channel[2];
	uint8_t activated = 0;
	uint32_t time_dial = 0;

public:
	Screen();
	void pushPower(float volt, float ampere, float watt, uint8_t ch);
	void drawScreen(dial_t dial);
	void activate(dial_t dial);
	void setMode(screen_mode_t mode);
	screen_mode_t getMode(void);
	int8_t dial_cnt = 0;
	int8_t dial_cnt_old;
	void countDial(int8_t mode_count, uint32_t milisec);
	void deActivate(uint8_t idx);
	void powerOn(uint8_t idx);
	uint32_t getTimeDial(void);
	void enterMode(void);
};
