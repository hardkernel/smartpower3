#include "screen.h"
#define WAIT 1
#define IWIDTH  138
#define IHEIGHT 48
#define W_HEADER 320
#define H_HEADER 40

#define W_CH0 20
#define OFFSET_CH0 15
#define H_VOLT (H_HEADER + OFFSET_CH0)
#define H_AMPERE (H_VOLT + IHEIGHT + OFFSET_CH0)
#define H_WATT (H_AMPERE + IHEIGHT + OFFSET_CH0)

#define OFFSET_X 0
#define OFFSET_Y H_HEADER

Channel::Channel(TFT_eSPI *tft, uint16_t x, uint16_t y)
{
	this->x = x + OFFSET_X;
	this->y = y + OFFSET_Y;
	volt = new Component(tft, IWIDTH, IHEIGHT, FONT_SEVEN_SEGMENT);
	ampere = new Component(tft, IWIDTH, IHEIGHT, FONT_SEVEN_SEGMENT);
	watt = new Component(tft, IWIDTH, IHEIGHT, FONT_SEVEN_SEGMENT);
}

void Channel::init(void)
{
	volt->init(TFT_RED, TFT_BLACK, 1, TR_DATUM);
	volt->setCoordinate(this->x, this->y);
	volt->draw(0.0);
	//tft_volt.pushSprite(W_CH0, H_VOLT);

	ampere->init(TFT_RED, TFT_BLACK, 1, TR_DATUM);
	ampere->setCoordinate(this->x, this->y + H_VOLT);
	ampere->draw(0.0);
	//tft_ampere.pushSprite(W_CH0, H_AMPERE);

	watt->init(TFT_RED, TFT_BLACK, 1, TR_DATUM);
	watt->setCoordinate(this->x, this->y + H_AMPERE);
	watt->draw(0.0);
	//tft_watt.pushSprite(W_CH0, H_WATT);
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


void Channel::drawVoltage(float val)
{
	volt->draw(val);
}

void Channel::drawAmpere(float val)
{
	ampere->draw(val);
}

void Channel::drawWatt(float val)
{
	watt->draw(val);
}


Screen::Screen()
{
	tft.init();
	tft.setRotation(3);
	tft.fillScreen(TFT_DARKGREY);
	pinMode(TFT_BL, OUTPUT);
	digitalWrite(TFT_BL, HIGH);

	/*
	initSprite(&tft_header, W_HEADER, H_HEADER, TFT_RED, TFT_GREEN, TL_DATUM, 1);
	drawHeader("SP3");
	*/
	header = new Component(&this->tft, W_HEADER, H_HEADER, 4);
	header->init(TFT_RED, TFT_BLACK, 2, TL_DATUM);
	header->draw("SP3", 0, 0);

	initBaseMode();
	channel[0] = new Channel(&tft, 10, 10);

	/*
	watt = new Component(&this->tft, IWIDTH, IHEIGHT, FONT_SEVEN_SEGMENT);
	watt->init(TFT_RED, TFT_BLACK, 1, TR_DATUM);
	watt->draw(0.0, 165, 100);

	delete watt;
	*/
}


void Screen::setMode(uint8_t val)
{
	if (mode != val) {
		mode = val;
		changeMode(val);
	}
}

uint8_t Screen::getMode(void)
{
	return mode;
}

void Screen::changeMode(uint8_t val)
{
	switch (val) {
	case 0:
		initBaseMode();
		break;
	}
}

void Screen::initBaseMode(void)
{
	/*
	initSprite(&tft_volt, IWIDTH, IHEIGHT, TFT_RED, TFT_BLACK, TR_DATUM, 1);
	initSprite(&tft_ampere, IWIDTH, IHEIGHT, TFT_RED, TFT_BLACK, TR_DATUM, 1);
	initSprite(&tft_watt, IWIDTH, IHEIGHT, TFT_RED, TFT_BLACK, TR_DATUM, 1);

	watt = new Component(&this->tft, IWIDTH, IHEIGHT, FONT_SEVEN_SEGMENT);
	watt->init(TFT_RED, TFT_BLACK, 1, TR_DATUM);
	watt->draw(0.0, 165, 100);
	*/
}

void Screen::drawHeader(String s)
{
	/*
	tft_header.drawString(s, 0, 0, 4);
	tft_header.pushSprite(0, 0);
	delay(WAIT);
	*/
}

void Screen::drawVoltage(float val, uint8_t ch)
{
	channel[ch]->drawVoltage(val);
}

void Screen::drawAmpere(float val, uint8_t ch)
{
	channel[ch]->drawAmpere(val);
}


void Screen::drawWatt(float val, uint8_t ch)
{
	channel[ch]->drawWatt(val);
}
