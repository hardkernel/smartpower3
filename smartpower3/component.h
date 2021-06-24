#ifndef COMPONENT_H
#define COMPONENT_H

#include <TFT_eSPI.h>

#define WAIT 1
#define W_SEG 140
#define H_SEG 48
#define OFFSET_SEG 35
#define OFFSET_CH 15

#define W_HEADER 470
#define H_HEADER 40

#define W_CH0 20

#define FONT_SEVEN_SEGMENT 7
#define OFFSET_X 0
#define OFFSET_Y H_HEADER

class Component
{
public:
	Component(TFT_eSPI *tft);
	Component(TFT_eSPI *tft, uint16_t width, uint16_t height, uint8_t font);
	~Component(void);

	void init(uint16_t fg_color, uint16_t bg_color, uint8_t size, uint8_t align);
	void draw(bool force_update = false);
	void clear(void);
	void draw(String s);
	void setCoordinate(uint16_t x, uint16_t y);
	void pushValue(uint16_t value);
	void activate(void);
	void deActivate(void);
	void setTextColor(uint16_t fg_color, uint16_t bg_color);
private:
	uint16_t width;
	uint16_t height;
	uint16_t x;
	uint16_t y;
	uint8_t font;
	bool activated;
	TFT_eSprite *img;
	TFT_eSPI *tft;
	uint16_t value;
	uint16_t value_old;
	void drawOutLines(void);
	void clearOutLines(void);
	uint8_t align = TL_DATUM;
};

#endif
