#ifndef COMPONENT_H
#define COMPONENT_H

#include <TFT_eSPI.h>
#include "helpers.h"

#define WAIT 1
#define H_SEG 48
#define OFFSET_SEG 35
#define OFFSET_CH 15

#define W_HEADER 470
#define H_HEADER 40

#define W_CH0 20

#define FONT_SEVEN_SEGMENT 7
#define OFFSET_X 0
#define OFFSET_Y H_HEADER

#define BG_DISABLED     0xCE79      /* 11001 110011 11001 , 204, 204, 204 */
#define FG_DISABLED     0x632C      /* 01100 011001 01100, 204, 204, 204 */

#define BG_ENABLED     0x4D9      /* 00000 100110 11001 , 204, 204, 204 */
#define FG_ENABLED     0xFFFF      /* 01100 011001 01100, 204, 204, 204 */
#define BG_ENABLED_INT    TFT_RED      /* 01100 011001 01100, 204, 204, 204 */
#define FG_ENABLED_INT    TFT_WHITE      /* 01100 011001 01100, 204, 204, 204 */
#define SELECTION_BORDER_WIDTH 2

class Component
{
public:
	Component(TFT_eSPI *tft);
	Component(TFT_eSPI *tft, uint16_t width, uint16_t height, uint8_t font);
	~Component(void);

	void init(uint16_t fg_color, uint16_t bg_color, uint8_t size, uint8_t align);
	void draw(bool force_update = false);
	void draw(String s);
	void draw(const char[]);
	void clear(void);
	void setCoordinate(uint16_t x, uint16_t y);
	void pushValue(uint16_t value);
	void select(uint16_t rectangle_color=TFT_YELLOW);
	void deSelect(uint16_t rectangle_color=BG_COLOR);
	void setTextColor(uint16_t fg_color, uint16_t bg_color);
	void loadFont(const uint8_t font[]);
	void unloadFont(void);
	//void clearAndDrawWithFont(const uint8_t font[], String string_to_draw);
	void clearAndDrawWithColoredFont(const uint8_t font[], uint16_t fg_color, const char* string_to_draw);
	void clearAndDrawWithFont(const uint8_t font[], const char* string_to_draw);
private:
	uint16_t width;
	uint16_t height;
	uint16_t x;
	uint16_t y;
	uint8_t font;
	bool selected;
	TFT_eSprite *img;
	TFT_eSPI *tft;
	uint16_t value;
	uint16_t value_old;
	void drawOutLines(uint16_t rectangle_color=TFT_YELLOW);
	void clearOutLines(uint16_t rectangle_color=BG_COLOR);
	uint8_t align = TL_DATUM;
};

#endif
