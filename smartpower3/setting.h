#include <Arduino.h>
#include <TFT_eSPI.h>
#include "component.h"

#define FAN		12
#define BL_LCD	17
#define FREQ	50000
#define RESOLUTION	8

class Setting
{
public:
	Setting(TFT_eSPI *tft);
	void init(uint16_t x, uint16_t y);

	uint8_t setBacklightLevel(void);
	uint8_t getBacklightLevel(void);
	uint8_t setFanLevel(void);
	uint8_t getFanLevel(void);

	void changeBacklight(uint8_t level=255);
	void changeFan(uint8_t level=255);
	void setBacklightLevel(uint8_t level);
	void setFanLevel(uint8_t level);

	void activateBLLevel(uint16_t color=TFT_YELLOW);
	void deActivateBLLevel(uint16_t color=TFT_BLACK);
	void activateFanLevel(uint16_t color=TFT_YELLOW);
	void deActivateFanLevel(uint16_t color=TFT_BLACK);

	void drawBacklightLevel(uint8_t level);
	void drawFanLevel(uint8_t level);
private:
	TFT_eSPI *tft;
	uint16_t x;
	uint16_t y;
	uint8_t backlight_level = 0;
	uint8_t backlight_level_edit = 0;
	uint8_t fan_level = 0;
	uint8_t fan_level_edit = 0;
	uint8_t bl_value[7] = {10, 25, 50, 75, 100, 125, 150};
	uint8_t fan_value[7] = {0, 50, 75, 100, 125, 150, 175};
};
