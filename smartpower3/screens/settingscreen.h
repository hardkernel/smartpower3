#include <Arduino.h>
#include <TFT_eSPI.h>
#include "component.h"
#include "settings.h"
#include "header.h"
#include "screens/screen.h"


#define BL_LCD 17
#define FREQ 50000
#define RESOLUTION 8

#define BL_LEVEL_WIDTH 277
#define BL_LEVEL_HEIGHT 22
#define BL_LEVEL_SPACING 1
#define X_BL_LEVEL 180
#define Y_BL_LEVEL -3

#define Y_LOGGING 60
#define Y_PADDING_LOG 30

#define Y_SERIAL 60
#define X_BAUD_RATE 225

#define X_LOG_LEVEL 250
#define Y_WIFI_INFO	140
#define X_IPADDR 226
#define X_SSID 172

/*#define LINE_SPACING 42
#define RIGHT_POSITION_X 460*/


enum setting_screen_mode_t {
	SETTING_SETTING = 0,
	SETTING_SETTING_BL,
	SETTING_SETTING_BAUD_RATE,
	SETTING_SETTING_LOG_INTERVAL,
	SETTING_SETTING_OPERATION_MODE,
};


enum screen_state_setting {
	STATE_SETTING_WIFI_ICON = 1,
	STATE_SETTING_LOGGING_ICON,
	STATE_OPERATION_MODE,
	STATE_LOG_INTERVAL,
	STATE_BAUD_RATE,
	STATE_BL,
};


class SettingScreen : public Screen
{
public:
	SettingScreen(TFT_eSPI *tft, Header *header, Settings *settings, WiFiManager *wifi_manager, uint8_t *onoff);

	virtual bool draw(void);
	virtual void init(void);
	virtual void select(void);
	virtual void deSelect(void);

	virtual void onShutdown(void);
	virtual void onWakeup(void);
	virtual screen_t getType(void);

	uint8_t setBacklightLevelPreset(void);
	void setBacklightLevelPreset(uint8_t level_preset, bool edit);
	void setBacklightLevel(uint8_t level);
	void turnOffBacklight(void);
	uint8_t setLogInterval(void);
	void setLogInterval(uint8_t val);
	device_operation_mode setOperationMode();
	uint16_t getEnabledLogInterval(void);

	uint8_t getBacklightLevel(void);
	uint8_t getLogInterval(void);
	uint16_t getLogIntervalValue(void);
	uint32_t getSerialBaud(void);
	uint32_t setSerialBaud(uint32_t baud);
	uint32_t setSerialBaud();
	uint8_t getSerialBaudIndex(void);
	uint16_t getOperationMode(void);

	void changeBacklight(uint8_t level=255);
	void restoreBacklight();
	void changeLogInterval(uint8_t log_interval, bool color_changed_value=false);
	void changeSerialBaud(uint8_t baud_level);
	void restoreSerialBaud(void);
	void restoreLogIntervalValue(void);
	void restoreOperationMode(void);
	void changeOperationMode(device_operation_mode operation_mode);

	void selectBLLevel(uint16_t rectangle_color=COLOR_RECTANGLE_SELECTED);
	void selectLogInterval(uint16_t text_color=COLOR_TEXT_SELECTED, uint16_t rectangle_color=COLOR_RECTANGLE_SELECTED);
	void selectSerialBaud(uint16_t text_color=COLOR_TEXT_SELECTED, uint16_t rectangle_color=COLOR_RECTANGLE_SELECTED);
	void selectOperationMode(uint16_t text_color=COLOR_TEXT_SELECTED, uint16_t rectangle_color=COLOR_RECTANGLE_SELECTED);

	void deSelectBLLevel(uint16_t rectangle_color=COLOR_RECTANGLE_DESELECTED);
	void deSelectLogInterval(uint16_t text_color=COLOR_TEXT_DESELECTED, uint16_t rectangle_color=COLOR_RECTANGLE_DESELECTED);
	void deSelectSerialBaud(uint16_t text_color=COLOR_TEXT_DESELECTED, uint16_t rectangle_color=COLOR_RECTANGLE_DESELECTED);
	void deSelectOperationMode(uint16_t text_color=COLOR_TEXT_DESELECTED, uint16_t rectangle_color=COLOR_RECTANGLE_DESELECTED);

	void drawBacklightLevel(uint8_t level);
	void drawLogIntervalValue(uint16_t log_interval);
	void drawSerialBaud(uint32_t serial_baud);
	void drawSerialLoggingEnabled(bool is_enabled=true);
	void drawWifiLoggingEnabled(bool is_enabled=true);
	//void popUp(void);

	void drawUDPIpaddr(const char* ipaddr);
	void drawUDPport(uint16_t port);
	void drawSSID(const char* ssid);
	void drawMode(device_operation_mode operation_mode = OPERATION_MODE_DEFAULT);

	void selectAndDrawSSIDText(void);
	void drawUDPIpaddrAndPort(void);
	void onEnter(void);
	void debug(void);
private:
	setting_screen_mode_t mode = SETTING_SETTING;
	//TFT_eSprite *popup;
	uint16_t x;
	uint16_t y;
	uint8_t backlight_level_preset = 0;
	uint8_t backlight_level_edit = 0;
	uint16_t log_interval = 0;
	uint16_t log_interval_edit = 0;
	uint8_t bl_value_preset[7] = {10, 25, 50, 75, 100, 125, 150};
	uint16_t log_value[7] = {0, 5, 10, 50, 100, 500, 1000};
	uint32_t serial_value[10] = {9600, 19200, 38400, 57600, 115200, 230400, 460800, 500000, 576000, 921600};
	uint32_t serial_baud = 0;
	uint32_t serial_baud_edit = 0;
	device_operation_mode operation_mode;
	device_operation_mode operation_mode_edit;
	//device_operation_mode operation_mode = OPERATION_MODE_DEFAULT;
	Component *com_serial_baud;
	Component *com_log_interval;
	Component *com_ssid;
	Component *com_udp_ipaddr;
	Component *com_udp_port;
	Component *com_mode;
	uint8_t _setBacklightLevelPreset(uint8_t level_preset);
	void drawSetting(void);
	void drawSettingBL(void);
	void drawSettingBaudRate(void);
	void drawSettingLogInterval(void);
	void drawOperationMode(void);
};
