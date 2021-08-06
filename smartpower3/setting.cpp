#include "setting.h"

Setting::Setting(TFT_eSPI *tft)
{
	this->tft = tft;
	lcd_bl = new Component(tft, 170, 26, 4);

	ledcSetup(2, FREQ, RESOLUTION);
	ledcAttachPin(BL_LCD, 2);

	//ledcWrite(2, bl_value[setting->getBacklightLevel()]);
}

void Setting::init(uint16_t x, uint16_t y)
{
	this->x = x;
	this->y = y;
	lcd_bl->init(TFT_BLACK, TFT_RED, 1, TL_DATUM);
	lcd_bl->setCoordinate(x, y);
	lcd_bl->draw("Backlight Level");

	tft->fillRect(x + 200, y, 135, 26, TFT_BLACK);
	tft->drawRect(x + 200, y-1, 135, 28, TFT_YELLOW);
	changeBacklight(backlight_level);
}

void Setting::setBacklightLevel(uint8_t level)
{
	if (level > 6)
		level = 6;
	else if (level < 0)
		level = 0;
	backlight_level = level;
	ledcWrite(2, bl_value[level]);
}

uint8_t Setting::setBacklightLevel(void)
{
	return backlight_level = backlight_level_edit;
}

uint8_t Setting::getBacklightLevel(void)
{
	return backlight_level;
}

void Setting::changeBacklight(uint8_t level)
{
	if (level == 255) {
		level = backlight_level;
	}
	drawBacklightLevel(level);
	backlight_level_edit = level;
	ledcWrite(2, bl_value[level]);
}

void Setting::drawBacklightLevel(uint8_t level)
{
	tft->fillRect(x + 202, y+1, 130, 24, TFT_BLACK);
	for (int i = 0; i < level; i++) {
		tft->fillRect(x + 202 + (i*22), y + 1, 20, 24, TFT_YELLOW);
	}
}

void Setting::activate()
{
//	lcd_bl->activate();
}

void Setting::activateBLLevel(uint16_t color)
{
	for (int i = 1; i < 4; i++)
		tft->drawRect(x + 200 -i, y-1-i, 135+i*2, 28+i*2, color);
}

void Setting::deActivateBLLevel(uint16_t color)
{
	for (int i = 1; i < 4; i++)
		tft->drawRect(x + 200 -i, y-1-i, 135+i*2, 28+i*2, color);
}
