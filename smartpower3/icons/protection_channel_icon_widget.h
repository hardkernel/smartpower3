#pragma once
#ifndef SMARTPOWER3_ICONS_PROTECTION_CHANNEL_ICON_WIDGET_H
#define SMARTPOWER3_ICONS_PROTECTION_CHANNEL_ICON_WIDGET_H

#include <TFT_eSPI.h>
#include "icon.h"
#include "component.h"
#include "channel_icon_widget.h"

class ProtectionChannelIconWidget: public ChannelIconWidget {
public:
	ProtectionChannelIconWidget(TFT_eSPI *tft, uint8_t channel, icon_image_enum icon_image);
	virtual ~ProtectionChannelIconWidget(void);
	virtual const uint16_t iconWidth(void);
	virtual const uint16_t iconHeight(void);
	virtual const uint8_t* iconCharArray(void);
	void update(bool forced = 0);
protected:
	icon_image_enum icon_image;
};

#endif
