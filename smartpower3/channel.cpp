#include "channel.h"

Channel::Channel(TFT_eSPI *tft)
{
	this->tft = tft;

	volt = new FndWidget(tft);
	ampere = new FndWidget(tft);
	watt = new FndWidget(tft);
#if 0
	this->tft = tft;
	this->x = x + OFFSET_CH + 20;
	this->y = y + OFFSET_CH + H_HEADER;
	volt = new Component(tft, W_SEG, H_SEG, 7);
	ampere = new Component(tft, W_SEG, H_SEG, 7);
	watt = new Component(tft, W_SEG, H_SEG, 7);

	_volt = new Component(tft, 64, 22, 4);
	_ampere = new Component(tft, 64, 22, 4);
#endif
}

Channel::~Channel(void)
{
#if 0
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
#endif
}

void Channel::init(uint16_t x, uint16_t y)
{
	volt->fnd_init(NUM_OF_FND, 2, true, x, Y_VOLT + y, FG_COLOR, BG_COLOR);
	ampere->fnd_init(NUM_OF_FND, 2, true, x, Y_AMPERE + y, FG_COLOR, BG_COLOR);
	watt->fnd_init(NUM_OF_FND, 2, true, x, Y_WATT + y, FG_COLOR, BG_COLOR);

	tft->drawString("V", x + W_SEG-8, y + 38, 4);
	tft->drawString("A", x + W_SEG-8, y + 29 + H_SEG + OFFSET_SEG, 4);
	tft->drawString("W", x + W_SEG-8, y + 20 + H_SEG*2 + OFFSET_SEG*2, 4);
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
	this->volt_set = this->volt_set + volt_set*10;
}

uint16_t Channel::getVolt(void)
{
	return volt_set;
}

void Channel::editVolt(float volt_set)
{
	this->_volt_set = this->volt_set + volt_set*10;
}

void Channel::setAmpereLimit(float ampere_limit)
{
	this->ampere_limit = this->ampere_limit + ampere_limit/10;
}

void Channel::editAmpereLimit(float ampere_limit)
{
	this->_ampere_limit = this->ampere_limit + ampere_limit/10;
}


void Channel::drawPower(void)
{
	volt->fnd_update();
	ampere->fnd_update();
	watt->fnd_update();
}

void Channel::clearCompColor(void)
{
	volt->setTextColor(TFT_RED, TFT_BLACK);
	ampere->setTextColor(TFT_RED, TFT_BLACK);
}

void Channel::setCompColor(comp_t comp)
{
	if (comp == VOLT)
		volt->setTextColor(TFT_YELLOW, TFT_BLACK);
	else if (comp == AMPERE)
		ampere->setTextColor(TFT_YELLOW, TFT_BLACK);
}

void Channel::pushPower(uint16_t volt, uint16_t ampere, uint16_t watt)
{
	this->volt->pushValue(volt);
	this->ampere->pushValue(ampere);
	this->watt->pushValue(watt);
}

void Channel::pushPowerEdit(float volt, float ampere, float watt)
{
	this->volt->pushValue(_volt_set);
	this->ampere->pushValue(_ampere_limit);
	this->watt->pushValue(watt);
}

void Channel::activate(comp_t comp)
{
	switch (comp) {
		case VOLT:
			volt->activate();
			break;
		case AMPERE:
			ampere->activate();
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
			ampere->deActivate();
			break;
		case WATT:
			break;
	}
}
