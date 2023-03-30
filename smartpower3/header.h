#pragma once
#ifndef HEADER_H
#define HEADER_H

#include "component.h"
#include "fndwidget.h"
#include "icons/base_icon_widget.h"
#include "icons/wifi_header_icon_widget.h"
#include "icons/log_header_icon_widget.h"
#include "icons/input_header_icon_widget.h"
#include "icons/unit_header_icon_widget.h"

class Header : public Component
{
public:
	Header(TFT_eSPI *tft);
	void init(uint16_t x, uint16_t y);
	void activate(void);
	void deActivate(void);
	void drawMode(String str);
	void pushPower(uint16_t volt, uint16_t ampere, uint16_t watt);
	void draw(void);
	bool getLowInput(void);
	void setLowInput(bool low_input);
	uint16_t getInputVoltage(void);
	void setDebug(void);
	void onLogging(void);
	void offLogging(void);
	void onWiFi(void);
	void offWiFi(void);
	Component *display_mode;

private:
	TFT_eSPI *tft;
	uint16_t x;
	uint16_t y;
	uint16_t in_volt = 0;
	uint16_t in_ampere = 0;
	bool updated;
	bool low_input = 1;
	uint8_t intPin = -1;
	Component *mode;
	InputHeaderIconWidget *icon_input;
	WifiHeaderIconWidget *icon_wifi;
	LogHeaderIconWidget *icon_log;
	UnitHeaderIconWidget *icon_v, *icon_a;
	FndWidget *input_v, *input_a;
	uint16_t debug = 0;
	bool flag_logging = false;
	bool update_logging_icon = false;
	bool flag_wifi = false;
	bool update_wifi_icon = false;
};

#endif
