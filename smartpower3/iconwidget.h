#pragma once
#ifndef ICONWIDGET_H
#define ICONWIDGET_H

#include <TFT_eSPI.h>
#include "icon.h"
#include "component.h"

struct icon {
	/* fnd position */
	uint16_t		x, y;
	uint16_t	width, height;

	// FND ripple blanking control
	uint16_t		bg_color, fg_color;
	bool			refresh;
	uint8_t nr_icon;
};

class IconWidget
{
public:
	IconWidget(TFT_eSPI *tft, uint8_t channel=0);
	~IconWidget(void);
	void clearAll(void);
	void setIconColor(uint16_t fg_color, uint16_t bg_color);
	void inputWrite(void);
	void wifiWrite(void);
	void logWrite(void);
	void polarityWrite(bool polarity);
	void unitsWrite(uint8_t nr_unit);
	void update(bool forced=0, uint8_t channel=0);
	struct icon* init (uint8_t nr_icon, uint16_t x, uint16_t y,
			uint16_t fg_color, uint16_t bg_color);
	void setInt(bool val);
	void clearInt(void);
	bool getInt(void);
private:
	struct icon *ic;
	TFT_eSPI *tft;
	bool value = false;
	uint8_t channel;
};

#endif
