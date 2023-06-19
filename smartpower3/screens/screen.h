#ifndef SMARTPOWER3_SCREEN_H_
#define SMARTPOWER3_SCREEN_H_

#include "header.h"
#include "settings.h"
#include "wifimanager.h"

#define STATE_NONE 0

#define SETTING_SCREEN_INIT_POSITION_X 10
#define SETTING_SCREEN_INIT_POSITION_Y 80

#define COLOR_RECTANGLE_SELECTED TFT_GREEN
#define COLOR_RECTANGLE_ACTIVATED TFT_YELLOW
#define COLOR_RECTANGLE_DESELECTED TFT_BLACK
#define COLOR_TEXT_SELECTED TFT_WHITE
#define COLOR_TEXT_ACTIVATED TFT_YELLOW
#define COLOR_TEXT_DESELECTED TFT_WHITE
#define COLOR_TEXT_WARNING TFT_YELLOW
#define COLOR_TEXT_ERROR TFT_RED

#define LINE_SPACING 42
#define RIGHT_POSITION_X 460


enum font_t {
	NOTOSANSBOLD20 = 0,
};


enum screen_t {
	LOGO_SCREEN = 0,
	VOLTAGE_SCREEN,
	SETTING_SCREEN,
	NETWORK_SCREEN,
	LAST_SCREEN_COUNT,  // added for easy counting of elements and other "hacks", should always stay at the end
};


class Screen
{
public:
	Screen();
	Screen(TFT_eSPI *tft, Header *header, Settings *settings, WiFiManager *wifi_manager, uint8_t onoff[]);
	virtual ~Screen();
	virtual bool draw(void) = 0;
	virtual void init(void) = 0;
	virtual void select(void);
	virtual void deSelect(void);
	virtual bool isInitialized(void);

	/*
	 * Executes for all screens (even the currently non-displayed ones) when the device is about to be put into sleep
	 * or low-power mode.
	 */
	virtual void onShutdown(void) = 0;
	/*
	 * Executed for all screens (even the currently non-displayed ones) when the device is waking up from sleep.
	 */
	virtual void onWakeup(void) = 0;
	/*
	 * Executes, for the current screen, when screen is about to be left - user moves to another screen.
	 */
	virtual void onLeave(void);
	/*
	 * Executes, for the current screen, when changed to from some other screen. Enables setup of screen.
	 * Note that this executes before screen is actually drawn on display, so drawing anything might lead to unexpected
	 * behaviour.
	 */
	virtual void onEnter(void);
	virtual screen_t getType(void) = 0;

	void pushButtonPress(uint8_t button_number, uint32_t dial_time, uint8_t flag_long_press);
	void pushTime(uint32_t cur_time);
	void countDial(int8_t mode_count, int8_t direct, uint8_t step, uint32_t milisec);
	void clearBtnEvent(void);
	void getBtnPress(uint8_t idx, uint32_t cur_time, bool long_pressed = false);
	void setTime(uint32_t milisec);

	void updateWifiInfo(void);
	const uint8_t* getFont(font_t font);
	void debug(void);
protected:
	TFT_eSPI *tft;
	Header *header;
	Settings *settings;
	WiFiManager *wifi_manager;
	uint8_t selected;
	uint32_t dial_time;
	int16_t dial_cnt = 0;
	int16_t dial_cnt_old;
	uint32_t cur_time = 0;
	int8_t dial_direct;
	uint8_t dial_state;
	uint8_t flag_long_press = false;
	uint32_t count_long_press = 0;
	bool btn_pressed[5] = {false,};
	bool show_next_screen = false;
	uint8_t *onoff;
	bool is_initialized = false;
	bool shutdown = false;
	bool updated_wifi_icon = false;
	bool updated_wifi_info = false;
};


#endif /* SMARTPOWER3_SCREEN_H_ */
