#pragma once
#ifndef SMARTPOWER3_ICONS_UNIT_HEADER_ICON_WIDGET_H_
#define SMARTPOWER3_ICONS_UNIT_HEADER_ICON_WIDGET_H_

#include <TFT_eSPI.h>
#include "icon.h"
#include "component.h"
#include "header_icon_widget.h"
#include "unit_icon_widget.h"

class UnitHeaderIconWidget : public HeaderIconWidget, UnitIconWidget {
public:
	UnitHeaderIconWidget(TFT_eSPI *tft, icon_image_enum icon_image);
	virtual ~UnitHeaderIconWidget(void);
	virtual const uint16_t iconWidth(void);
	virtual const uint16_t iconHeight(void);
	virtual const uint8_t* iconCharArray(void);
protected:
	icon_image_enum icon_image;
};

#endif
