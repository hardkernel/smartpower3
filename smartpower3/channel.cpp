#include "channel.h"

Channel::Channel(TFT_eSPI *tft, uint16_t x, uint16_t y)
{
	this->tft = tft;
	this->x = x + OFFSET_CH + 20;
	this->y = y + OFFSET_CH + H_HEADER;
	volt = new Component(tft, W_SEG, H_SEG, 7);
	ampere = new Component(tft, W_SEG, H_SEG, 7);
	watt = new Component(tft, W_SEG, H_SEG, 7);

	_volt = new Component(tft, 64, 22, 4);
	_ampere = new Component(tft, 64, 22, 4);
}

Channel::~Channel(void)
{
	delete volt;
	delete ampere;
	delete watt;
	delete _volt;
	delete _ampere;
	volt = NULL;
	ampere = NULL;
	watt = NULL;
	_volt = NULL;
	_ampere = NULL;
}

void Channel::init(void)
{
	volt->init(TFT_RED, TFT_BLACK, 1, TR_DATUM);
	volt->setCoordinate(x, y);
	volt->draw();
	tft->drawString("V", x + W_SEG, y + 20, 4);

	_volt->init(TFT_YELLOW, TFT_BLACK, 1, TR_DATUM);
	_volt->setCoordinate(x+76, y + H_SEG);
	_volt->draw();

	ampere->init(TFT_RED, TFT_BLACK, 1, TR_DATUM);
	ampere->setCoordinate(x, y + H_SEG + OFFSET_SEG);
	ampere->draw();
	tft->drawString("A", x + W_SEG, y + 20 + H_SEG + OFFSET_SEG, 4);

	_ampere->init(TFT_YELLOW, TFT_BLACK, 1, TR_DATUM);
	_ampere->setCoordinate(x+76, y + H_SEG*2 + OFFSET_SEG*1);
	_ampere->draw();

	watt->init(TFT_RED, TFT_BLACK, 1, TR_DATUM);
	watt->setCoordinate(x, y + H_SEG*2 + OFFSET_SEG*2);
	watt->draw();
	tft->drawString("W", x + W_SEG, y + 20 + H_SEG*2 + OFFSET_SEG*2, 4);
}

void Channel::powerOn(bool onoff)
{
	this->onoff = onoff;
}

void Channel::powerOn()
{
	onoff = !onoff;
}

bool Channel::getOnOff()
{
	return this->onoff;
}

void Channel::setVolt(float volt_set)
{
	//this->_volt_set = volt_set;
	this->volt_set = this->volt_set + volt_set/10;
}

void Channel::editVolt(float volt_set)
{
	this->_volt_set = this->volt_set + volt_set/10;
}

void Channel::setAmpereLimit(float ampere_limit)
{
	//this->ampere_limit = ampere_limit;
	this->ampere_limit = this->ampere_limit + ampere_limit/10;
}

void Channel::editAmpereLimit(float ampere_limit)
{
	this->_ampere_limit = this->ampere_limit + ampere_limit/10;
}


void Channel::drawPower(void)
{
	volt->draw();
	_volt->draw();
	ampere->draw();
	_ampere->draw();
	watt->draw();
}

void Channel::clearCompColor(void)
{
	volt->setTextColor(TFT_RED, TFT_BLACK);
	_volt->setTextColor(TFT_YELLOW, TFT_BLACK);
	ampere->setTextColor(TFT_RED, TFT_BLACK);
	_ampere->setTextColor(TFT_YELLOW, TFT_BLACK);
}

void Channel::setCompColor(comp_t comp)
{
	if (comp == VOLT) {
		volt->setTextColor(TFT_YELLOW, TFT_BLACK);
		_volt->setTextColor(TFT_RED, TFT_BLACK);
	} else if (comp == AMPERE) {
		ampere->setTextColor(TFT_YELLOW, TFT_BLACK);
		_ampere->setTextColor(TFT_RED, TFT_BLACK);
	}
}

void Channel::pushPower(float volt, float ampere, float watt)
{
	this->volt->pushValue(volt);
	this->ampere->pushValue(ampere);
	this->watt->pushValue(watt);
	this->_volt->pushValue(volt_set);
	this->_ampere->pushValue(ampere_limit);
}

void Channel::pushPowerEdit(float volt, float ampere, float watt)
{
	this->volt->pushValue(_volt_set);
	this->ampere->pushValue(_ampere_limit);
	this->watt->pushValue(watt);
	this->_volt->pushValue(volt);
	this->_ampere->pushValue(ampere);
}

void Channel::activate(comp_t comp)
{
	switch (comp) {
		case VOLT:
			//volt->activate();
			tft->drawRect(25, 60, 175, 80, TFT_YELLOW);
			break;
		case AMPERE:
			tft->drawRect(25, 140, 175, 80, TFT_YELLOW);
			//ampere->activate();
			break;
		case WATT:
			break;
	}
}

void Channel::deActivate(comp_t comp)
{
	switch (comp) {
		case VOLT:
			//volt->deActivate();
			tft->drawRect(25, 60, 175, 80, TFT_DARKGREY);
			break;
		case AMPERE:
			tft->drawRect(25, 140, 175, 80, TFT_DARKGREY);
			//ampere->deActivate();
			break;
		case WATT:
			break;
	}
}
