#include "screen.h"

Screen::Screen()
{
	tft.init();
	tft.setRotation(2);
	tft.fillScreen(TFT_DARKGREY);
	//tft.fillScreen(TFT_BLACK);
	pinMode(TFT_BL, OUTPUT);
	digitalWrite(TFT_BL, HIGH);

	header = new Component(&this->tft, W_HEADER, H_HEADER, 2);
	header->init(TFT_RED, TFT_BLACK, 1, TL_DATUM);
	header->setCoordinate(5, 5);
	header->draw("SP3");

	channel[0] = new Channel(&tft, 0, 10);
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
	Serial.println("BASE MODE");
	if (dial_cnt != dial_cnt_old) {
		clearBtnEvent();
		mode = BASE_MOVE;
	}
}

void Screen::drawBaseMove()
{
	Serial.println("BASE MOVE MODE");
	activate();
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
		if (this->activated == 1) { // POWER
			mode = BASE_EDIT;
			channel[0]->setCompColor(VOLT);
		} else if (this->activated == 2) {
			mode = BASE_EDIT;
			channel[0]->setCompColor(AMPERE);
		}
		dial_cnt = 0;
		dial_cnt_old = 0;
		btn_pressed[3] = false;
	}
}

void Screen::drawBaseEdit()
{
	Serial.println("BASE EDIT MODE");
	if ((cur_time - dial_time) > 10000) {
		mode = BASE;
		deActivate(0);
		channel[0]->clearCompColor();
	}
	if (btn_pressed[2] == true) {
		mode = BASE_MOVE;
		channel[0]->clearCompColor();
		btn_pressed[2] = false;
	}
	if (btn_pressed[3] == true) {
		mode = BASE_MOVE;
		channel[0]->clearCompColor();
		btn_pressed[3] = false;
		if (this->activated == 1) {
			channel[0]->setVolt(dial_cnt);
		} else if (this->activated == 2) {
			channel[0]->setAmpereLimit(dial_cnt);
		}
	}
	Serial.println(dial_cnt);
	if (dial_cnt != dial_cnt_old) {
		Serial.println("dialing");
		dial_cnt_old = dial_cnt;
		if (this->activated == 1) {
			channel[0]->editVolt(dial_cnt);
		} else if (this->activated == 2) {
			channel[0]->editAmpereLimit(dial_cnt);
		}
	}
}

void Screen::drawScreen()
{
	switch (mode) {
	case BASE:
		drawBase();
		header->draw("SP3 MODE : BASE");
		break;
	case BASE_MOVE:
		header->draw("SP3 MODE : DIAL");
		drawBaseMove();
		break;
	case BASE_EDIT:
		header->draw("SP3 MODE : EDIT");
		drawBaseEdit();
		break;
	}
	channel[0]->drawPower();
}

void Screen::deActivate(uint8_t idx)
{
	this->activated = 0;
	//header->deActivate();
	channel[0]->deActivate(VOLT);
	channel[0]->deActivate(AMPERE);
	tft.drawRect(3, 3, 235, 50, TFT_DARKGREY);
}

void Screen::activate()
{
	if (dial_cnt == dial_cnt_old)
		return;
	dial_cnt_old = dial_cnt;

	switch (abs(dial_cnt%3)) {
		case 0:
			this->activated = 0;
			tft.drawRect(3, 3, 235, 50, TFT_YELLOW);
			//header->activate();
			channel[0]->deActivate(VOLT);
			channel[0]->deActivate(AMPERE);
			break;
		case 1:
			this->activated = 1;
			//header->deActivate();
			tft.drawRect(3, 3, 235, 50, TFT_DARKGREY);
			channel[0]->activate(VOLT);
			channel[0]->deActivate(AMPERE);
			break;
		case 2:
			this->activated = 2;
			//header->deActivate();
			tft.drawRect(3, 3, 235, 50, TFT_DARKGREY);
			channel[0]->deActivate(VOLT);
			channel[0]->activate(AMPERE);
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

void Screen::clearBtnEvent(void)
{
	for (int i = 0; i < 4; i++)
		btn_pressed[i] = false;
}

void Screen::getBtnPress(uint8_t idx, uint32_t cur_time)
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

bool Screen::getOnOff(uint8_t idx)
{
	return this->channel[idx]->getOnOff();
}
