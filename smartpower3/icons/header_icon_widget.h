#pragma once
#ifndef SMARTPOWER3_ICONS_HEADER_ICON_WIDGET_H_
#define SMARTPOWER3_ICONS_HEADER_ICON_WIDGET_H_

#include <TFT_eSPI.h>
#include "icon.h"
#include "component.h"
#include "base_icon_widget.h"

class HeaderIconWidget: public BaseIconWidget {
public:
	HeaderIconWidget(TFT_eSPI *tft);
	virtual ~HeaderIconWidget(void);
protected:
	bool value = false;
};

#endif
