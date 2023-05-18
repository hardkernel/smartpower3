#pragma once
#ifndef SMARTPOWER3_ICONS_UNIT_CHANNEL_ICON_WIDGET_H
#define SMARTPOWER3_ICONS_UNIT_CHANNEL_ICON_WIDGET_H

#include <TFT_eSPI.h>
#include "icon.h"
#include "component.h"
#include "channel_icon_widget.h"
#include "unit_icon_widget.h"

class UnitChannelIconWidget : public ChannelIconWidget, UnitIconWidget
{
public:
	UnitChannelIconWidget(TFT_eSPI *tft, uint8_t channel, icon_image_enum icon_image);
	virtual ~UnitChannelIconWidget(void);
	virtual const uint8_t* iconCharArray(void);
};

#endif
