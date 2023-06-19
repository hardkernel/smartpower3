#include "screens/screen.h"
#include "fonts/NotoSansBold20.h"


Screen::Screen()
{
}

Screen::Screen(TFT_eSPI *tft, Header *header, Settings *settings, WiFiManager *wifi_manager, uint8_t *onoff)
{
	this->tft = tft;
	this->header = header;
	this->settings = settings;
	this->wifi_manager = wifi_manager;
	this->onoff = onoff;
}

Screen::~Screen()
{
}

void Screen::select(void)
{
}

void Screen::deSelect(void)
{
}

void Screen::debug()
{
	header->setDebug();
}

void Screen::clearBtnEvent(void)
{
	for (int i = 0; i < 4; i++)
		btn_pressed[i] = false;
}

void Screen::countDial(int8_t dial_cnt, int8_t direct, uint8_t step, uint32_t milisec)
{
	this->dial_cnt += dial_cnt*step;
	this->dial_time = milisec;
	this->dial_direct = direct;
}

void Screen::getBtnPress(uint8_t idx, uint32_t cur_time, bool long_pressed)
{
	switch (idx) {
	case 0:  // Channel0 ON/OFF
	case 1:  // Channel1 ON/OFF
		if (shutdown || this->getType() != VOLTAGE_SCREEN || long_pressed)
			break;
		if (onoff[idx] == 1)
			onoff[idx] = 2;
		else if (onoff[idx] == 0)
			onoff[idx] = 3;
		btn_pressed[idx] = true;
		break;
	case 2:  // MENU/CANCEL
		dial_time = cur_time;
		flag_long_press = 1;
		count_long_press = long_pressed;
		break;
	case 3:  // Set value - dial button
		if (shutdown)
			break;
		dial_time = cur_time;
		btn_pressed[idx] = true;
		break;
	default:
		break;
	}
}

void Screen::setTime(uint32_t milisec)
{
	if (flag_long_press) {
		if (digitalRead(36) == 0) {
			if (count_long_press) {
				count_long_press = false;
				btn_pressed[4] = true;
				flag_long_press = 0;
			}
		} else {
			if (flag_long_press != 3) {
				btn_pressed[2] = true;
			}
			flag_long_press = 0;
		}
		if (flag_long_press == 1) {
			flag_long_press = 2;
		}
	}
	this->cur_time = milisec;
}

void Screen::pushButtonPress(uint8_t button_number, uint32_t dial_time, uint8_t flag_long_press)
{
	this->btn_pressed[button_number] = true;
	this->dial_time = dial_time;
	this->flag_long_press = flag_long_press;
}

void Screen::pushTime(uint32_t cur_time)
{
	this->cur_time = cur_time;
}

bool Screen::isInitialized(void)
{
	return this->is_initialized;
}

void Screen::onLeave(void)
{
	this->is_initialized = false;
}

void Screen::onEnter(void)
{
}

void Screen::updateWifiInfo(void)
{
	this->updated_wifi_info = true;
	this->updated_wifi_icon = true;
}

/*
 * A little trick method to prevent multiple inclusion of font header files (whole fonts) in individual screens.
 * Otherwise the preprocessor would replace the include with the whole contents of font array in every file
 * the screen header file is included in.
 */
const uint8_t* Screen::getFont(font_t font_name)
{
	if (font_name == NOTOSANSBOLD20) {
		return NotoSansBold20;
	} else {
		return 0x00;
	}
}
