#include <Arduino.h>
#include <TFT_eSPI.h>
#include "component.h"

#define PWM_FAN 12
#define BL_LCD	17
#define FREQ	5000
#define RESOLUTION	8

class Setting
{
public:
	Setting(TFT_eSPI *tft);
	void init(uint16_t x, uint16_t y);
	void activate(void);
	void deActivate(void);
	void changeBacklight(uint8_t level=255);
	void setBacklightLevel(uint8_t level);
	uint8_t setBacklightLevel(void);
	uint8_t getBacklightLevel(void);
	void activateBLLevel(uint16_t color=TFT_YELLOW);
	void deActivateBLLevel(uint16_t color=TFT_BLACK);
	void drawBacklightLevel(uint8_t level);
private:
	TFT_eSPI *tft;
	uint16_t x;
	uint16_t y;
	uint8_t backlight_level = 0;
	uint8_t backlight_level_edit = 0;
	Component *lcd_bl;
	uint8_t bl_value[7] = {10, 25, 50, 75, 100, 125, 150};
};
