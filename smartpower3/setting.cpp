#include "setting.h"

Setting::Setting(TFT_eSPI *tft)
{
	this->tft = tft;

	ledcSetup(2, FREQ, RESOLUTION);
	ledcAttachPin(BL_LCD, 2);

	ledcSetup(3, FREQ, RESOLUTION);
	ledcAttachPin(FAN, 3);
}

void Setting::init(uint16_t x, uint16_t y)
{
	this->x = x;
	this->y = y;

	tft->loadFont("NotoSans-Bold20");
	tft->drawString("Build date : ", x + 140, y + 195, 2);
	tft->drawString(String(__DATE__), x + 260, y + 195, 2);
	tft->drawString(String(__TIME__), x + 380, y + 195, 2);
	tft->unloadFont();

	tft->loadFont("Chewy-Regular32");
	tft->drawString("Backlight Level", x, y, 4);
	tft->drawString("Fan Level", x, y + 50, 4);
	tft->drawString("Logging", x, y + 100, 4);
	tft->unloadFont();

	tft->fillRect(x + X_BL_LEVEL, y, 135, 26, TFT_BLACK);
	tft->drawRect(x + X_BL_LEVEL, y-1, 135, 28, TFT_YELLOW);
	changeBacklight(backlight_level);

	tft->fillRect(x + X_FAN_LEVEL, y + 50, 135, 26, TFT_BLACK);
	tft->drawRect(x + X_FAN_LEVEL, y-1 + 50, 135, 28, TFT_YELLOW);
	changeFan(fan_level);

	tft->fillRect(x + X_LOG_LEVEL, y + 100, 135, 26, TFT_BLACK);
	tft->drawRect(x + X_LOG_LEVEL, y-1 + 100, 135, 28, TFT_YELLOW);
	changeLogInterval(log_interval);
}

uint8_t Setting::setBacklightLevel(void)
{
	return backlight_level = backlight_level_edit;
}

uint8_t Setting::getBacklightLevel(void)
{
	return backlight_level;
}

uint8_t Setting::setFanLevel(void)
{
	return fan_level = fan_level_edit;
}

uint8_t Setting::getFanLevel(void)
{
	return fan_level;
}

uint16_t Setting::setLogInterval(void)
{
	return log_interval = log_interval_edit;
}

uint16_t Setting::getLogIntervalValue(void)
{
	return log_value[log_interval];
}

uint16_t Setting::getLogInterval(void)
{
	return log_interval;
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

void Setting::changeFan(uint8_t level)
{
	if (level == 255) {
		level = fan_level;
	}
	drawFanLevel(level);
	fan_level_edit = level;
	ledcWrite(3, fan_value[level]);
}

void Setting::changeLogInterval(uint16_t log_interval)
{
	drawLogInterval(log_value[log_interval]);
	log_interval_edit = log_interval;
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

void Setting::setFanLevel(uint8_t level)
{
	if (level > 6)
		level = 6;
	else if (level < 0)
		level = 0;
	fan_level = level;
	ledcWrite(3, fan_value[level]);
}

void Setting::activateBLLevel(uint16_t color)
{
	for (int i = 1; i < 4; i++)
		tft->drawRect(x + X_BL_LEVEL-i, y-1-i, 135+i*2, 28+i*2, color);
}

void Setting::deActivateBLLevel(uint16_t color)
{
	for (int i = 1; i < 4; i++)
		tft->drawRect(x + X_BL_LEVEL -i, y-1-i, 135+i*2, 28+i*2, color);
}

void Setting::activateFanLevel(uint16_t color)
{
	for (int i = 1; i < 4; i++)
		tft->drawRect(x + X_FAN_LEVEL -i, y-1-i + 50, 135+i*2, 28+i*2, color);
}

void Setting::deActivateFanLevel(uint16_t color)
{
	for (int i = 1; i < 4; i++)
		tft->drawRect(x + X_FAN_LEVEL -i, y-1-i + 50, 135+i*2, 28+i*2, color);
}

void Setting::activateLogInterval(uint16_t color)
{
	for (int i = 1; i < 4; i++)
		tft->drawRect(x + X_LOG_LEVEL -i, y-1-i + 100, 135+i*2, 28+i*2, color);
}

void Setting::deActivateLogInterval(uint16_t color)
{
	for (int i = 1; i < 4; i++)
		tft->drawRect(x + X_LOG_LEVEL -i, y-1-i + 100, 135+i*2, 28+i*2, color);
}


void Setting::drawBacklightLevel(uint8_t level)
{
	tft->fillRect(x + X_BL_LEVEL + 2, y+1, 130, 24, TFT_BLACK);
	for (int i = 0; i < level; i++) {
		tft->fillRect(x + X_BL_LEVEL + 2 + (i*22), y + 1, 20, 24, TFT_YELLOW);
	}
}

void Setting::drawFanLevel(uint8_t level)
{
	tft->fillRect(x + X_FAN_LEVEL + 2, y+1 + 50, 130, 24, TFT_BLACK);
	for (int i = 0; i < level; i++) {
		tft->fillRect(x + X_FAN_LEVEL + 2 + (i*22), y + 1 + 50, 20, 24, TFT_YELLOW);
	}
}

void Setting::drawLogInterval(uint16_t log_value)
{
	tft->fillRect(x + X_LOG_LEVEL + 2, y+1 + 100, 130, 24, TFT_BLACK);
	if (log_value == 0)
		tft->drawString("OFF", x + X_LOG_LEVEL + 5, y + 100, 4);
	else
		tft->drawString(String(log_value) + " ms", x + X_LOG_LEVEL + 5, y + 100, 4);
}
