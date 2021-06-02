#pragma once
#ifndef FNDWIDGET_H
#define FNDWIDGET_H

#include <TFT_eSPI.h>
#include "fnd_font.h"
#include "component.h"

#define WAIT 1
#define W_SEG 140
#define H_SEG 48
#define OFFSET_SEG 35
#define OFFSET_CH 15

#define W_HEADER 230
#define H_HEADER 40

#define W_CH0 20

#define FONT_SEVEN_SEGMENT 7
#define OFFSET_X 0
#define OFFSET_Y H_HEADER

class FndWidget : public Component
{
public:
	FndWidget(TFT_eSPI *tft);
	~FndWidget(void);

	void init(uint16_t fg_color, uint16_t bg_color, uint8_t size, uint8_t align);
	void draw(void);
	void draw(String s);
	void setCoordinate(uint16_t x, uint16_t y);
	void pushValue(uint16_t value);
	void activate(void);
	void deActivate(void);
	void setTextColor(uint16_t fg_color, uint16_t bg_color);
	struct fnd* fnd_init (uint8_t cnt, uint8_t dot_pos, bool rbo,
			uint16_t x, uint16_t y, uint16_t fg_color, uint16_t bg_color);
	void fnd_dot_write(void);
	void fnd_num_write(uint16_t f_pos);
	void fnd_clear_all(void);
	void fnd_update();
	void fnd_fb_write(int8_t f_pos, int8_t f_value, int16_t color);
	void fnd_dd_clear(void);
private:
	uint8_t mode = 0;
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	uint8_t font;
	TFT_eSPI *tft;
	uint16_t value;
	uint16_t old_value;
	float value_old;
	void drawOutLines(void);
	void clearOutLines(void);
	struct fnd *f;
};

#endif
