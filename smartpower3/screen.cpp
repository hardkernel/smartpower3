#include "screen.h"

Screen::Screen()
{
	tft.init();
	tft.setRotation(2);
	tft.fillScreen(TFT_DARKGREY);
	pinMode(TFT_BL, OUTPUT);
	digitalWrite(TFT_BL, HIGH);

	header = new Component(&this->tft, W_HEADER, H_HEADER, 4);
	header->init(TFT_RED, TFT_BLACK, 1, TL_DATUM);
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
	if (mode == BASE_EDIT)
		channel[ch]->pushPowerEdit(volt, ampere, watt);
	else
		channel[ch]->pushPower(volt, ampere, watt);
}

void Screen::powerOn(uint8_t idx)
{
	channel[idx]->powerOn();
}

void Screen::drawBase()
{
	if (dial_cnt != dial_cnt_old) {
		dial_cnt_old = dial_cnt;
		Serial.printf("dial count : %d, abs : %d\n", dial_cnt, abs(dial_cnt%3));
		mode = BASE_MOVE;
	}
	channel[0]->drawPower();
}

void Screen::drawBaseMove()
{
	if ((cur_time - dial_time) > 5000) {
		mode = BASE;
		deActivate(0);
	}
	if (btn_pressed[2] == true) {
		mode = BASE;
		btn_pressed[2] = false;
		deActivate(0);
	}
	if (btn_pressed[3] == true) {
		/*
		if (isActivated(POWER))
			mode = BASE_EDIT;
		*/
		btn_pressed[2] = false;
	}
}

void Screen::drawBaseEdit()
{
	if ((cur_time - dial_time) > 10000) {
		mode = BASE;
		deActivate(0);
	}
	if (btn_pressed[2] == true) {
		mode = BASE_MOVE;
		btn_pressed[2] = false;
		// draw cancel state
	}
	if (btn_pressed[3] == true) {
		mode = BASE_MOVE;
		btn_pressed[2] = false;
		// draw set state
	}
}

void Screen::drawScreen()
{
	switch (mode) {
	case BASE:
		drawBase();
	case BASE_MOVE:
		drawBaseMove();
	case BASE_EDIT:
		drawBaseEdit();
	}
}
void Screen::deActivate(uint8_t idx)
{
	this->activated = HEADER;
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

void Screen::activate()
{
	if (dial_cnt == dial_cnt_old)
		return;
	dial_cnt_old = dial_cnt;
	Serial.printf("dial count : %d, abs : %d\n", dial_cnt, abs(dial_cnt%3));

	switch (abs(dial_cnt%2)) {
		case 0:
			header->activate();
			channel[0]->deActivate(VOLT);
			//channel[1]->deActivate(VOLT);
			this->activated = HEADER;
			break;
		case 1:
			channel[0]->activate(VOLT);
			header->deActivate();
			//channel[1]->deActivate(VOLT);
			this->activated = VOLT;
			break;
		case 2:
			//channel[1]->activate(VOLT);
			header->deActivate();
			channel[0]->deActivate(VOLT);
			break;
	}
}

void Screen::countDial(int8_t dial_cnt, uint32_t milisec)
{
	this->dial_cnt += dial_cnt;
	this->dial_time = milisec;
}

uint32_t Screen::setTime(uint32_t milisec)
{
	this->cur_time = milisec;
}

void Screen::getBtnPress(uint8_t idx)
{
	Serial.printf("button pressed %d\n\r", idx);
	btn_pressed[idx] = true;
	switch (idx) {
	case 0: /* Channel0 ON/OFF */
	case 1: /* Channel1 ON/OFF */
		powerOn(idx);
		break;
	case 2: /* MENU/CANCEL */
		break;
	case 3: /* Set value */
		break;
	}
}

