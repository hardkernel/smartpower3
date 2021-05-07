#include "channel.h"

Channel::Channel(TFT_eSPI *tft, uint16_t x, uint16_t y)
{
	this->tft = tft;
	this->x = x + OFFSET_CH;
	this->y = y + OFFSET_CH + H_HEADER;
	volt = new Component(tft, W_SEG, H_SEG, 7);
	ampere = new Component(tft, W_SEG, H_SEG, 7);
	watt = new Component(tft, W_SEG, H_SEG, 7);
}

Channel::~Channel(void)
{
	delete volt;
	delete ampere;
	delete watt;
	volt = NULL;
	ampere = NULL;
	watt = NULL;
}

void Channel::init(void)
{
	volt->init(TFT_RED, TFT_BLACK, 1, TR_DATUM);
	volt->setCoordinate(x, y);
	volt->draw();

	ampere->init(TFT_RED, TFT_BLACK, 1, TR_DATUM);
	ampere->setCoordinate(x, y + H_SEG + OFFSET_CH);
	ampere->draw();

	watt->init(TFT_RED, TFT_BLACK, 2, TR_DATUM);
	watt->setCoordinate(x, y + H_SEG*2 + OFFSET_CH*2);
	watt->draw();
}

void Channel::clearOutLines(uint8_t idx)
{
	uint16_t x, y, w, h;
	switch (idx) {
		case 0:
			x = this->x;
			y = this->y;
			w = W_SEG;
			h = H_SEG*3 + OFFSET_CH*2;
			break;
		case 1:
			x = this->x;
			y = this->y;
			w = W_SEG;
			h = H_SEG;
			break;
		case 2:
			x = this->x;
			y = this->y + H_SEG + OFFSET_CH;
			w = W_SEG;
			h = H_SEG;
			break;
		case 3:
			x = this->x;
			y = this->y + H_SEG*2 + OFFSET_CH*2;
			w = W_SEG;
			h = H_SEG;
			break;
	}
				
	for (int i = 0; i < 3; i++) {
		tft->drawRect(x-(i+2), y-(i+2), w+(4+i*2), h+(4 +2*i), TFT_BLACK);
	}
}

void Channel::drawOutLines(uint8_t idx)
{
	uint16_t x, y, w, h;
	switch (idx) {
		case 0:
			x = this->x;
			y = this->y;
			w = W_SEG;
			h = H_SEG*3 + OFFSET_CH*2;
			break;
		case 1:
			x = this->x;
			y = this->y;
			w = W_SEG;
			h = H_SEG;
			break;
		case 2:
			x = this->x;
			y = this->y + H_SEG + OFFSET_CH;
			w = W_SEG;
			h = H_SEG;
			break;
		case 3:
			x = this->x;
			y = this->y + H_SEG*2 + OFFSET_CH*2;
			w = W_SEG;
			h = H_SEG;
			break;
	}
				
	for (int i = 0; i < 3; i++) {
		tft->drawRect(x-(i+2), y-(i+2), w+(4+i*2), h+(4 +2*i), TFT_YELLOW);
	}
}

void Channel::powerOn(bool onoff)
{
	this->onoff = onoff;
}

void Channel::powerOn()
{
	onoff = !onoff;
}

void Channel::setVolt(float volt_set)
{
	this->volt_set = volt_set;
}

void Channel::drawPower(void)
{
	if (onoff) {
		volt->draw();
		ampere->draw();
		watt->draw();
	} else {
		pushPower(0, 0, 0);
		volt->draw();
		ampere->draw();
		watt->draw();
	}
}

void Channel::pushPower(float volt, float ampere, float watt)
{
	this->volt->pushValue(volt);
	this->ampere->pushValue(ampere);
	this->watt->pushValue(watt);
}

void Channel::activate(comp_t comp)
{
	switch (comp) {
		case VOLT:
			volt->activate();
			break;
		case AMPERE:
			break;
		case WATT:
			break;
	}
}

void Channel::deActivate(comp_t comp)
{
	switch (comp) {
		case VOLT:
			volt->deActivate();
			break;
		case AMPERE:
			break;
		case WATT:
			break;
	}
}
