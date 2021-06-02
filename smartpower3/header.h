#pragma once
#ifndef HEADER_H
#define HEADER_H

#include "component.h"

class Header : public Component
{
public:
	Header(TFT_eSPI *tft, uint16_t width, uint16_t height, uint8_t font);

};

#endif
