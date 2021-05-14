#include <Arduino.h>
#include <TFT_eSPI.h>
#include "channel.h"

enum screen_mode_t {
	BASE,
	BASE_MOVE,
	BASE_EDIT,
	SETTING
};

class Screen
{
private:
	TFT_eSPI tft = TFT_eSPI();
	screen_mode_t mode = BASE;
	Component *header;
	Channel *channel[2];
	uint8_t activated = 0;
	uint32_t dial_time = 0;
	uint32_t cur_time = 0;
	bool btn_pressed[4] = {false,};
	int8_t dial_cnt = 0;
	int8_t dial_cnt_old;
	void drawBase(void);
	void drawBaseEdit(void);
	void drawBaseMove(void);

public:
	Screen();
	void pushPower(float volt, float ampere, float watt, uint8_t ch);
	void drawScreen();
	void activate();
	void countDial(int8_t mode_count, uint32_t milisec);
	void deActivate(uint8_t idx);
	void powerOn(uint8_t idx);
	void getBtnPress(uint8_t idx, uint32_t cur_time);
	uint32_t setTime(uint32_t milisec);
	void clearBtnEvent(void);
	bool getOnOff(uint8_t idx);
};
