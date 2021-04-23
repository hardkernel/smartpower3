#include "screen.h"

Screen::Screen()
{
	tft.init();
	tft.setRotation(3);
	tft.fillScreen(TFT_DARKGREY);
	pinMode(TFT_BL, OUTPUT);
	digitalWrite(TFT_BL, HIGH);

	initSprite(&tft_header, W_HEADER, H_HEADER, TFT_RED, TFT_GREEN, TL_DATUM, 1);
	drawHeader("SP3");
	initMode0();
}

void Screen::initInstances()
{
	tft_volt.deleteSprite();
	tft_watt.deleteSprite();
	tft_ampere.deleteSprite();
}

void Screen::setMode(uint8_t val)
{
	if (mode != val) {
		mode = val;
		//initInstances();
		changeMode(val);
	}
}

uint8_t Screen::getMode(void)
{
	return mode;
}

void Screen::initSprite(TFT_eSprite *sprite, uint16_t width, uint16_t height,
		uint16_t fg_color, uint16_t bg_color, uint8_t align, uint8_t size)
{
	sprite->setColorDepth(8);
	sprite->createSprite(width, height);
	sprite->fillSprite(TFT_BLACK);
	sprite->setTextSize(size);
	sprite->setTextColor(fg_color, bg_color);
	sprite->setTextDatum(align);
}

void Screen::changeMode(uint8_t val)
{
	switch (val) {
	case 0:
		initMode0();
		break;
	}
}

void Screen::initMode0(void)
{
	initSprite(&tft_volt, IWIDTH, IHEIGHT, TFT_RED, TFT_BLACK, TR_DATUM, 1);
	initSprite(&tft_ampere, IWIDTH, IHEIGHT, TFT_RED, TFT_BLACK, TR_DATUM, 1);
	initSprite(&tft_watt, IWIDTH, IHEIGHT, TFT_RED, TFT_BLACK, TR_DATUM, 1);
}

void Screen::drawHeader(String s)
{
	tft_header.drawString(s, 0, 0, 4);
	tft_header.pushSprite(0, 0);
	delay(WAIT);
}

void Screen::drawVoltage(float volt)
{
	tft_volt.drawString(String(volt, 2), IWIDTH, 0, 7);
	tft_volt.pushSprite(W_CH0, H_VOLT);
	delay(WAIT);
}

void Screen::drawAmpere(float ampere)
{
	tft_ampere.drawString(String(ampere, 2), IWIDTH, 0, 7);
	tft_ampere.pushSprite(W_CH0, H_AMPERE);
	delay(WAIT);
}

void Screen::drawWatt(float watt)
{
	tft_watt.drawString(String(watt, 2), IWIDTH, 0, 7);
	tft_watt.pushSprite(W_CH0, H_WATT);
	delay(WAIT);
}
