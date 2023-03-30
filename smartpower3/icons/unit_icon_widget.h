#pragma once
#ifndef SMARTPOWER3_ICONS_UNIT_ICON_WIDGET_H_
#define SMARTPOWER3_ICONS_UNIT_ICON_WIDGET_H_

#include <TFT_eSPI.h>
#include "icon.h"
#include "component.h"
#include "channel_icon_widget.h"

class UnitIconWidget
{
public:
	UnitIconWidget(icon_image_enum icon_image);
	virtual ~UnitIconWidget(void);
	virtual const uint8_t* iconCharArray(void);
protected:
	icon_image_enum icon_image;
};

#endif
