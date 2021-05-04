#include "screen.h"

Screen::Screen()
{
	tft.init();
	tft.setRotation(3);
	tft.fillScreen(TFT_DARKGREY);
	pinMode(TFT_BL, OUTPUT);
	digitalWrite(TFT_BL, HIGH);

	header = new Component(&this->tft, W_HEADER, H_HEADER, 4);
	header->init(TFT_RED, TFT_BLACK, 2, TL_DATUM);
	header->setCoordinate(5, 5);
	header->draw("SP3");

	initBaseMode();
	channel[0] = new Channel(&tft, 10, 10);
	channel[0]->init();
	channel[1] = new Channel(&tft, 20 + W_SEG, 10);
	channel[1]->init();
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


void Screen::activated(uint8_t idx)
{
	switch (idx) {
		case 0:
			header->activate();
		case 1:
			//channel[1]->volt->activate();
			break;
		case 2:
			//channel[1]->ampere->activate();
			break;
	}

}
