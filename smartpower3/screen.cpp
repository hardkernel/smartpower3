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

	header = new Component(&tft, 470, H_HEADER, 2);
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
	else
		channel[ch]->pushPower(volt, ampere, watt);
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
		Serial.println("BASE MOVE");
	}
}

void Screen::drawBaseMove()
{
	activate();
	if ((cur_time - dial_time) > 5000) {
		mode = BASE;
		deActivate(0);
		Serial.println("BASE");
	}
	if (btn_pressed[2] == true) {
		mode = BASE;
		btn_pressed[2] = false;
		deActivate(0);
		Serial.println("BASE");
	}
	if (btn_pressed[3] == true) {
		if (channel[0] == NULL)
			return;
		if (this->activated == 1) { // POWER
			mode = BASE_EDIT;
			channel[0]->setCompColor(VOLT);
			Serial.println("BASE_EDIT volt");
		} else if (this->activated == 2) {
			mode = BASE_EDIT;
			Serial.println("BASE_EDIT ampere");
			channel[0]->setCompColor(AMPERE);
		}
		dial_cnt = 0;
		dial_cnt_old = 0;
		btn_pressed[3] = false;
	}
}

void Screen::drawBaseEdit()
{
	if ((channel[0] == NULL) or (channel[1] == NULL))
		return;
	if ((cur_time - dial_time) > 10000) {
		mode = BASE;
		Serial.println("BASE");
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
			stpd01_ch0.setVoltage(channel[0]->getVolt());
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
	//stpd01_ch0.monitorInterrupt(0);
	switch (mode) {
	case BASE:
		drawBase();
		if (header != NULL)
			header->draw("SP3 MODE : BASE");
		break;
	case BASE_MOVE:
		if (header != NULL)
			header->draw("SP3 MODE : DIAL");
		drawBaseMove();
		break;
	case BASE_EDIT:
		if (header != NULL)
			header->draw("SP3 MODE : EDIT");
		drawBaseEdit();
		break;
	}
	if ((channel[0] == NULL) or (channel[1] == NULL))
		return;
	channel[0]->drawPower();
	channel[1]->drawPower();
}

void Screen::deActivate(uint8_t idx)
{
	this->activated = 0;
	//header->deActivate();
	if (channel[idx] == NULL)
		return;
	channel[0]->deActivate(VOLT);
	channel[0]->deActivate(AMPERE);
	tft.drawRect(3, 3, 235, 50, TFT_DARKGREY);
}

void Screen::activate()
{
	if (dial_cnt == dial_cnt_old)
		return;
	dial_cnt_old = dial_cnt;
	Serial.printf("dial_cnt : %d, mod : %d\n", dial_cnt, abs(dial_cnt%3));

	switch (abs(dial_cnt%3)) {
		case 0:
			this->activated = 0;
			header->activate();
			if (channel[0] == NULL)
				return;
			channel[0]->deActivate(VOLT);
			channel[0]->deActivate(AMPERE);
			break;
		case 1:
			this->activated = 1;
			header->deActivate();
			if (channel[0] == NULL)
				return;
			channel[0]->activate(VOLT);
			channel[0]->deActivate(AMPERE);
			break;
		case 2:
			this->activated = 2;
			header->deActivate();
			if (channel[0] == NULL)
				return;
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
		powerOn(idx);
		stpd01_ch0.onOff();
		break;
	case 1: /* Channel1 ON/OFF */
		powerOn(idx);
		stpd01_ch1.onOff();
		break;
	case 2:  /* MENU/CANCEL */
		break;
	case 3: /* Set value */
		break;
	}
}

bool Screen::getOnOff(uint8_t idx)
{
	if (channel[idx] == NULL)
		return NULL;
	return channel[idx]->getOnOff();
}
