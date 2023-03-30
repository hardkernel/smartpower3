#pragma once
#ifndef SMARTPOWER3_ICONS_BASE_ICON_WIDGET_H
#define SMARTPOWER3_ICONS_BASE_ICON_WIDGET_H

#include <TFT_eSPI.h>
#include "icon.h"
#include "component.h"

enum icon_image_enum {
	PROTECTION_OVERVOLTAGE = 0,
	PROTECTION_CONSTANT_CURRENT = 1,
	PROTECTION_SHORT_CIRCUIT = 2,
	PROTECTION_TEMPERATURE = 3,
	PROTECTION_TEMPERATURE_WARNING = 4,
	PROTECTION_INDUCTOR_PEAK = 5,
	LARGE_VOLT = 6,
	LARGE_AMPERE = 7,
	LARGE_WATT = 8,
	SMALL_VOLT = 9,
	SMALL_AMPERE = 10
};

#define ICON_DIMENSION_PORTRAIT_WIDTH 16;
#define ICON_DIMENSION_PORTRAIT_HEIGHT 32;
#define ICON_DIMENSION_LANDSCAPE_WIDTH 32;
#define ICON_DIMENSION_LANDSCAPE_HEIGHT 16;
#define ICON_DIMENSION_LARGE_SQUARE 32;


struct icon {
	uint16_t x, y;
	uint16_t width, height;
	uint16_t bg_color, fg_color;
	bool refresh;
};

class BaseIconWidget {
public:
	BaseIconWidget(TFT_eSPI *tft);
	virtual ~BaseIconWidget(void);
	virtual struct icon* init(uint16_t x, uint16_t y, uint16_t fg_color,
			uint16_t bg_color);
	virtual void draw(void);
	virtual const uint16_t iconWidth(void);
	virtual const uint16_t iconHeight(void);
	virtual const uint8_t* iconCharArray(void);
	void clearAll(void);
	void setIconColor(uint16_t fg_color, uint16_t bg_color);
protected:
	struct icon *ic;
	TFT_eSPI *tft;
};

#endif
