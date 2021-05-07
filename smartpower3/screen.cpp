#include "screen.h"

Screen::Screen()
{
	tft.init();
	tft.setRotation(2);
	tft.fillScreen(TFT_DARKGREY);
	pinMode(TFT_BL, OUTPUT);
	digitalWrite(TFT_BL, HIGH);

	header = new Component(&this->tft, W_HEADER, H_HEADER, 4);
	header->init(TFT_RED, TFT_BLACK, 2, TL_DATUM);
	header->setCoordinate(5, 5);
	header->draw("SP3");

	channel[0] = new Channel(&tft, 10, 10);
	channel[0]->init();
	channel[0]->powerOn(false);
	/*
	channel[1] = new Channel(&tft, 20 + W_SEG, 10);
	channel[1]->init();
	channel[1]->powerOn(false);
	*/
}

void Screen::pushPower(float volt, float ampere, float watt, uint8_t ch)
{
	channel[ch]->pushPower(volt, ampere, watt);
}

void Screen::powerOn(uint8_t idx)
{
	channel[idx]->powerOn();
}

void Screen::drawScreen(dial_t dial)
{
	switch (mode) {
	case BASE_MOVE:
		activate(dial);
	case BASE:
		channel[0]->drawPower();
		//channel[1]->drawPower();
		break;
	case BASE_EDIT:
		break;
	case SETTING:
		break;
	}
}

void Screen::deActivate(uint8_t idx)
{
	switch (idx) {
		case 0:
			header->deActivate();
		case 1:
			channel[0]->deActivate(VOLT);
			break;
		case 2:
			//channel[1]->deActivate(VOLT);
			break;
	}
}

void Screen::activate(dial_t dial)
{
	if (mode_count == mode_count_old)
		return;
	mode_count_old = mode_count;
	Serial.printf("mode_count : %d, abs : %d\n", mode_count, abs(mode_count%3));

	switch (abs(mode_count%2)) {
		case 0:
			header->activate();
			channel[0]->deActivate(VOLT);
			//channel[1]->deActivate(VOLT);
			break;
		case 1:
			channel[0]->activate(VOLT);
			header->deActivate();
			//channel[1]->deActivate(VOLT);
			break;
		case 2:
			//channel[1]->activate(VOLT);
			header->deActivate();
			channel[0]->deActivate(VOLT);
			break;
	}
}

void Screen::setModeCounter(int8_t mode_count)
{
	this->mode_count += mode_count;
}

void Screen::setMode(screen_mode_t mode)
{
	this->mode = mode;
}

screen_mode_t Screen::getMode(void)
{
	return mode;
}
