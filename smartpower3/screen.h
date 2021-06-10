#include <Arduino.h>
#include <TFT_eSPI.h>
#include "channel.h"
#include "header.h"
#include <STPD01.h>

enum screen_mode_t {
	BASE,
	BASE_MOVE,
	BASE_EDIT, SETTING
};

enum state {
	STATE_HEADER = 0,
	STATE_VOLT1,
	STATE_AMPERE1,
	STATE_AMPERE0,
	STATE_VOLT0,
};

class Screen
{
public:
	Screen();
	void begin(TwoWire *theWire = &Wire);
	void pushPower(uint16_t volt, uint16_t ampere, uint16_t watt, uint8_t ch);
	void drawScreen();
	void activate();
	void countDial(int8_t mode_count, bool direct, uint32_t milisec);
	void deActivate();
	void powerOn(uint8_t idx);
	void getBtnPress(uint8_t idx, uint32_t cur_time);
	void setTime(uint32_t milisec);
	void clearBtnEvent(void);
	bool getOnOff(uint8_t idx);
	STPD01 *stpd01[2];
	static void isr_stp(void);
	static void hello(void *param);
private:
	TFT_eSPI tft = TFT_eSPI();
	screen_mode_t mode = BASE;
	Channel *channel[2];
	Header *header;
	TwoWire *_wire;
	uint8_t activated = 0;
	uint32_t dial_time = 0;
	uint32_t cur_time = 0;
	bool btn_pressed[4] = {false,};
	int8_t dial_cnt = 0;
	int8_t dial_cnt_old;
	bool dial_direct;
	uint8_t dial_state;
	void drawBase(void);
	void drawBaseEdit(void);
	void drawBaseMove(void);

};
