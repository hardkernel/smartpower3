#pragma once
#ifndef SMARTPOWER3_ICONS_CHANNEL_ICON_WIDGET_H_
#define SMARTPOWER3_ICONS_CHANNEL_ICON_WIDGET_H_

#include <TFT_eSPI.h>
#include "icon.h"
#include "component.h"
#include "base_icon_widget.h"
#include "unit_icon_widget.h"

class ChannelIconWidget: public BaseIconWidget {
public:
	ChannelIconWidget(TFT_eSPI *tft, uint8_t channel);
	virtual ~ChannelIconWidget(void);
	void setInt(bool val);
	void clearInt(void);
	bool getInt(void);
protected:
	bool value = false;
	uint8_t channel;
};

#endif
