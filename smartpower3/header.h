#pragma once
#ifndef HEADER_H
#define HEADER_H

#include "component.h"

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
	void lowIntPin();
	void highIntPin();
	void setDebug(void);

private:
	TFT_eSPI *tft;
	uint16_t x;
	uint16_t y;
	uint16_t in_volt = 0;
	uint16_t in_ampere = 0;
	bool updated;
	bool low_input = 1;
	uint8_t intPin = -1;
	Component *input;
	Component *int0;
	Component *mode;
	Component *display_mode;
	uint16_t debug = 0;
};

#endif
