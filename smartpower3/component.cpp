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

void Component::init(uint16_t fg_color, uint16_t bg_color, uint8_t text_size, uint8_t align)
{
	img->setColorDepth(16);
	img->fillSprite(BG_COLOR);
	img->createSprite(width, height);
	img->setTextColor(fg_color, bg_color);
	img->setTextSize(text_size);
	img->setTextDatum(align);
	this->align = align;
}

void Component::setTextColor(uint16_t fg_color, uint16_t bg_color)
{
	if ((img->textcolor != fg_color) || (img->textbgcolor != bg_color))
		img->setTextColor(fg_color, bg_color);
}

void Component::setCoordinate(uint16_t x, uint16_t y)
{
	this->x = x;
	this->y = y;
}

void Component::drawOutLines(uint16_t rectangle_color)
{
	for (int i = 0; i < SELECTION_BORDER_WIDTH; i++) {
		tft->drawRect(
				x-(SELECTION_BORDER_WIDTH-i),
				y-(SELECTION_BORDER_WIDTH-i),
				width+(SELECTION_BORDER_WIDTH*2-i*2),
				height+(SELECTION_BORDER_WIDTH*2-i*2),
				rectangle_color);
	}
}

void Component::clearOutLines(uint16_t rectangle_color)
{
	this->drawOutLines(rectangle_color);
}

void Component::loadFont(const uint8_t font[])
{
	img->loadFont(font);
}

void Component::unloadFont(void)
{
	img->unloadFont();
}

void Component::draw(const char s[])
{
	if (align == TR_DATUM)
		img->drawString(s, width, 0, font);
	else if (align == MR_DATUM)
		img->drawString(s, width, height/2, font);
	else
		img->drawString(s, width/2, height/2, font);
	img->pushSprite(x, y);
	delay(WAIT);
}

void Component::draw(String s)
{
	if (align == TR_DATUM)
		img->drawString(s, width, 0, font);
	else if (align == MR_DATUM)
		img->drawString(s, width, height/2, font);
	else
		img->drawString(s, width/2, height/2, font);
	img->pushSprite(x, y);
	delay(WAIT);
}

void Component::draw(bool force_update)
{
	if ((value == value_old) && !force_update)
		return;
	if ((value_old >= 10000) && (value < 10000))
		clear();
	value_old = value;
	if (align == TR_DATUM)
		img->drawString(String(value/1000.0, 1), width, 0, font);
	else
		img->drawString(String(value/1000.0, 1), 0, 0, font);
	img->pushSprite(x, y);
	delay(WAIT);
}

void Component::clear(void)
{
	img->fillSprite(BG_COLOR);
}

void Component::clearAndDrawWithColoredFont(const uint8_t font[], uint16_t fg_color, const char* string_to_draw)
{
	this->clear();
	this->loadFont(font);
	this->setTextColor(fg_color, BG_COLOR);
	this->draw(string_to_draw);
	this->unloadFont();
}

void Component::clearAndDrawWithFont(const uint8_t font[], const char* string_to_draw)
{
	this->clear();
	this->loadFont(font);
	this->draw(string_to_draw);
	this->unloadFont();
}

void Component::pushValue(uint16_t value)
{
	this->value = value;
}

void Component::select(uint16_t rectangle_color)
{
	selected = true;
	drawOutLines(rectangle_color);
}

void Component::deSelect(uint16_t rectangle_color)
{
	if (selected) {
		clearOutLines(rectangle_color);
		selected = false;
	}
}
