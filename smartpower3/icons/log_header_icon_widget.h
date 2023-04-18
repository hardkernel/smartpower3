#pragma once
#ifndef SMARTPOWER3_ICONS_LOG_HEADER_ICON_WIDGET_H_
#define SMARTPOWER3_ICONS_LOG_HEADER_ICON_WIDGET_H_

#include <TFT_eSPI.h>
#include "icon.h"
#include "component.h"
#include "header_icon_widget.h"

class LogHeaderIconWidget: public HeaderIconWidget {
public:
	LogHeaderIconWidget(TFT_eSPI *tft);
	virtual ~LogHeaderIconWidget(void);
	virtual const uint8_t* iconCharArray(void);
};

#endif
