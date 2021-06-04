#include "screen.h"

Screen::Screen()
{
	tft.init();
	tft.setRotation(3);
	tft.fillScreen(TFT_DARKGREY);
	tft.fillScreen(TFT_DARKGREY);
	tft.setSwapBytes(true);
	pinMode(TFT_BL, OUTPUT);
	digitalWrite(TFT_BL, HIGH);

	header = new Header(&tft, 470, H_HEADER, 2);
	header->init(TFT_RED, TFT_BLACK, 2, TL_DATUM);
	header->setCoordinate(5, 5);
	header->draw("SP3");

	channel[0] = new Channel(&tft);
	channel[0]->init(30, 60);
	channel[1] = new Channel(&tft);
	channel[1]->init(260, 60);
}

void Screen::begin(TwoWire *theWire)
{
	_wire = theWire;
	stpd01_ch0.begin(0x5, _wire);
	stpd01_ch1.begin(0x4, _wire);
	stpd01_ch0.setCurrentLimit(3000);
	stpd01_ch1.setCurrentLimit(3000);
}

void Screen::pushPower(uint16_t volt, uint16_t ampere, uint16_t watt, uint8_t ch)
{
	if (channel[ch] == NULL)
		return;
	if (mode == BASE_EDIT)
		channel[ch]->pushPowerEdit(volt, ampere, watt);
	else {
		channel[ch]->pushPower(volt, ampere, watt);
	}
}

void Screen::powerOn(uint8_t idx)
{
	if (channel[idx] == NULL)
		return;
	channel[idx]->powerOn();
}

void Screen::drawBase()
{
	if (dial_cnt != dial_cnt_old) {
		clearBtnEvent();
		mode = BASE_MOVE;
	}
}

void Screen::drawBaseMove()
{
	activate();
	if ((cur_time - dial_time) > 5000) {
		mode = BASE;
		deActivate();
		dial_cnt = 0;
		dial_cnt_old = 0;
	}
	if (btn_pressed[2] == true) {
		mode = BASE;
		btn_pressed[2] = false;
		deActivate();
		dial_cnt = 0;
		dial_cnt_old = 0;
	}
	if (btn_pressed[3] == true) {
		if (activated == STATE_VOLT0) {
			mode = BASE_EDIT;
			channel[0]->setCompColor(VOLT);
		} else if (activated == STATE_AMPERE0) {
			mode = BASE_EDIT;
			channel[0]->setCompColor(AMPERE);
		} else if (activated == STATE_VOLT1) {
			mode = BASE_EDIT;
			channel[1]->setCompColor(VOLT);
		} else if (activated == STATE_AMPERE1) {
			mode = BASE_EDIT;
			channel[1]->setCompColor(AMPERE);
		}
		dial_state = dial_cnt;
		dial_cnt = 0;
		dial_cnt_old = 0;
		btn_pressed[3] = false;
	}
}

void Screen::drawBaseEdit()
{
	if ((cur_time - dial_time) > 10000) {
		mode = BASE;
		deActivate();
		channel[0]->clearCompColor();
		channel[1]->clearCompColor();
	}
	if (btn_pressed[2] == true) {
		mode = BASE_MOVE;
		channel[0]->clearCompColor();
		channel[1]->clearCompColor();
		btn_pressed[2] = false;
	}
	if (btn_pressed[3] == true) {
		mode = BASE_MOVE;
		channel[0]->clearCompColor();
		channel[1]->clearCompColor();
		btn_pressed[3] = false;
		if (activated == STATE_VOLT0) {
			channel[0]->setVolt(dial_cnt);
			stpd01_ch0.setVoltage(channel[0]->getVolt());
		} else if (activated == STATE_AMPERE0) {
			channel[0]->setCurrentLimit(dial_cnt);
			stpd01_ch0.setCurrentLimit(channel[0]->getCurrentLimit());
		} else if (activated == STATE_VOLT1) {
			channel[1]->setVolt(dial_cnt);
			stpd01_ch1.setVoltage(channel[1]->getVolt());
		} else if (activated == STATE_AMPERE1) {
			channel[1]->setCurrentLimit(dial_cnt);
			stpd01_ch1.setCurrentLimit(channel[1]->getCurrentLimit());
		}
		dial_cnt = dial_state;
		return;
	}
	if (dial_cnt != dial_cnt_old) {
		dial_cnt_old = dial_cnt;
		if (activated == STATE_VOLT0) {
			channel[0]->editVolt(dial_cnt);
		} else if (activated == STATE_AMPERE0) {
			channel[0]->editCurrentLimit(dial_cnt);
		} else if (activated == STATE_VOLT1) {
			channel[1]->editVolt(dial_cnt);
		} else if (activated == STATE_AMPERE1) {
			channel[1]->editCurrentLimit(dial_cnt);
		}
	}
}

void Screen::drawScreen()
{
	stpd01_ch0.monitorInterrupt(0);
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
	channel[1]->drawPower();
}

void Screen::deActivate()
{
	activated = 0;
	header->deActivate();
	channel[0]->deActivate(VOLT);
	channel[0]->deActivate(AMPERE);
	channel[1]->deActivate(VOLT);
	channel[1]->deActivate(AMPERE);
}

void Screen::activate()
{
	if (dial_cnt == dial_cnt_old)
		return;
	dial_cnt_old = dial_cnt;
	if (dial_cnt > 4)
		dial_cnt = 0;
	else if (dial_cnt < 0)
		dial_cnt = 4;
	Serial.printf("dial_cnt : %d, mod : %d\n", dial_cnt);

	deActivate();
	activated = dial_cnt;
	switch (dial_cnt) {
		case STATE_HEADER:
			header->activate();
			break;
		case STATE_VOLT0:
			channel[0]->activate(VOLT);
			break;
		case STATE_AMPERE0:
			channel[0]->activate(AMPERE);
			break;
		case STATE_AMPERE1:
			channel[1]->activate(AMPERE);
			break;
		case STATE_VOLT1:
			channel[1]->activate(VOLT);
			break;
	}
}

void Screen::countDial(int8_t dial_cnt, bool direct, uint32_t milisec)
{
	this->dial_cnt += dial_cnt;
	this->dial_time = milisec;
	this->dial_direct = direct;
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
		powerOn(idx);
		stpd01_ch0.onOff();
		break;
	case 1: /* Channel1 ON/OFF */
		powerOn(idx);
		stpd01_ch1.onOff();
		break;
	case 2:  /* MENU/CANCEL */
		dial_time = cur_time;
		break;
	case 3: /* Set value */
		dial_time = cur_time;
		break;
	}
}

bool Screen::getOnOff(uint8_t idx)
{
	if (channel[idx] == NULL)
		return NULL;
	return channel[idx]->getOnOff();
}
