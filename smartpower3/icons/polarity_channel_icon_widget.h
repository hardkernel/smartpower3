#pragma once
#ifndef SMARTPOWER3_POLARITY_CHANNEL_ICON_WIDGET_H
#define SMARTPOWER3_POLARITY_CHANNEL_ICON_WIDGET_H

#include <TFT_eSPI.h>
#include "icon.h"
#include "component.h"
#include "channel_icon_widget.h"

enum icon_polarity {
	NEGATIVE = 0,
	POSITIVE = 1
};

class PolarityChannelIconWidget: public ChannelIconWidget {
public:
	PolarityChannelIconWidget(TFT_eSPI *tft, uint8_t channel, icon_polarity polarity);
	~PolarityChannelIconWidget(void);
	virtual const uint8_t* iconCharArray(void);
protected:
	icon_polarity polarity;
};

#endif
