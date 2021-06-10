#include "component.h"

Component::Component(TFT_eSPI *tft, uint16_t width, uint16_t height, uint8_t font)
{
	this->tft = tft;
	img = new TFT_eSprite(tft);
	this->width = width;
	this->height = height;
	this->font = font;
	value = 0;
}

Component::Component(TFT_eSPI *tft)
{
	this->tft = tft;
	value = 0;
}

Component::~Component(void)
{
	img->deleteSprite();
	delete img;
	img = NULL;
}

void Component::init(uint16_t fg_color, uint16_t bg_color, uint8_t size, uint8_t align)
{
	img->setColorDepth(8);
	img->fillSprite(TFT_BLACK);
	img->createSprite(width, height);
	img->setTextColor(fg_color, bg_color);
	img->setTextSize(size);
	img->setTextDatum(align);
}

void Component::setTextColor(uint16_t fg_color, uint16_t bg_color)
{
	img->setTextColor(fg_color, bg_color);
}

void Component::setCoordinate(uint16_t x, uint16_t y)
{
	this->x = x;
	this->y = y;
}

void Component::drawOutLines(void)
{
	for (int i = 0; i < 3; i++) {
		tft->drawRect(x-(3-i), y-(3-i), width+(6-i*2), height+(6-i*2), TFT_YELLOW);
	}
}

void Component::clearOutLines(void)
{
	for (int i = 0; i < 3; i++) {
		tft->drawRect(x-(3-i), y-(3-i), width+(6-i*2), height+(6-i*2), TFT_DARKGREY);
	}
}

void Component::draw(String s)
{
	img->drawString(s, 0, 0, font);
	img->pushSprite(x, y);
	delay(WAIT);
}

void Component::draw(void)
{
	if (value == value_old)
		return;
	value_old = value;
	if (value < 10)
		img->drawString("0" + String(value, 2), width, 0, font);
	else
		img->drawString(String(value, 2), width, 0, font);
	img->pushSprite(x, y);
	delay(WAIT);
}

void Component::pushValue(float value)
{
	this->value = value;
}

void Component::activate(void)
{
	activated = true;
	drawOutLines();
}

void Component::deActivate(void)
{
	if (activated) {
		clearOutLines();
		activated = false;
	}
}
