#include <Arduino.h>
#include <TFT_eSPI.h>
#include "component.h"

#define FAN		12
#define BL_LCD	17
#define FREQ	50000
#define RESOLUTION	8

#define X_BL_LEVEL 250
#define X_FAN_LEVEL 250
#define X_LOG_LEVEL 250

class Setting
{
public:
	Setting(TFT_eSPI *tft);
	void init(uint16_t x, uint16_t y);

	uint8_t setBacklightLevel(void);
	void setBacklightLevel(uint8_t level);
	uint8_t setFanLevel(void);
	void setFanLevel(uint8_t level);
	uint16_t setLogInterval(void);
	void setLogIntervalValue(uint16_t val);
	uint32_t setSerialBaud(uint32_t baud);
	uint32_t setSerialBaud();

	uint8_t getBacklightLevel(void);
	uint8_t getFanLevel(void);
	uint16_t getLogInterval(void);
	uint16_t getLogIntervalValue(void);
	uint32_t getSerialBaud(void);
	uint8_t getSerialBaudLevel(void);

	void changeBacklight(uint8_t level=255);
	void changeFan(uint8_t level=255);
	void changeLogInterval(uint16_t log_interval);
	void changeSerialBaud(uint8_t baud_level);
	void restoreSerialBaud(void);
	void restoreLogInterval(void);

	void activateBLLevel(uint16_t color=TFT_YELLOW);
	void activateFanLevel(uint16_t color=TFT_YELLOW);
	void activateLogInterval(uint16_t color=TFT_YELLOW);
	void activateSerialBaud(uint16_t color=TFT_YELLOW);
	void activateSerialLogging(uint16_t color=TFT_YELLOW);

	void deActivateBLLevel(uint16_t color=TFT_BLACK);
	void deActivateFanLevel(uint16_t color=TFT_BLACK);
	void deActivateLogInterval(uint16_t color=TFT_BLACK);
	void deActivateSerialBaud(uint16_t color=TFT_BLACK);
	void deActivateSerialLogging(uint16_t color=TFT_BLACK);

	void drawBacklightLevel(uint8_t level);
	void drawFanLevel(uint8_t level);
	void drawLogInterval(uint16_t log_interval);
	void drawSerialBaud(uint32_t serial_baud);
	void popUp(void);
	
	void activateSerialBaud(void);
	void availableLogInterval(void);
	void debug(void);
private:
	TFT_eSPI *tft;
	TFT_eSprite *popup;
	uint16_t x;
	uint16_t y;
	uint8_t backlight_level = 0;
	uint8_t backlight_level_edit = 0;
	uint8_t fan_level = 0;
	uint8_t fan_level_edit = 0;
	uint16_t log_interval = 0;
	uint16_t log_interval_edit = 0;
	uint8_t bl_value[7] = {10, 25, 50, 75, 100, 125, 150};
	uint8_t fan_value[7] = {0, 50, 75, 100, 125, 150, 175};
	uint16_t log_value[7] = {0, 5, 10, 50, 100, 500, 1000};
	uint32_t serial_value[10] = {9600, 19200, 38400, 57600, 115200, 230400, 460800, 500000, 576000, 921600};
	uint32_t serial_baud = 0;
	uint32_t serial_baud_edit = 0;
	Component *com_serial_baud;
	Component *com_log_interval;
};
