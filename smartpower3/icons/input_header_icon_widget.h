#pragma once
#ifndef SMARTPOWER3_ICONS_INPUT_HEADER_ICON_WIDGET_H
#define SMARTPOWER3_ICONS_INPUT_HEADER_ICON_WIDGET_H

#include <TFT_eSPI.h>
#include "icon.h"
#include "component.h"
#include "header_icon_widget.h"

class InputHeaderIconWidget: public HeaderIconWidget {
public:
	InputHeaderIconWidget(TFT_eSPI *tft);
	virtual ~InputHeaderIconWidget(void);
	virtual const uint8_t* iconCharArray(void);
};

#endif
