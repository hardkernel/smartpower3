#include "FS.h"
#include "SPIFFS.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "header.h"
#include <STPD01.h>
#include "wifimanager.h"
#include "settings.h"
#include "screens/screen.h"
#include "screens/voltagescreen.h"
#include "screens/logoscreen.h"
#include "screens/settingscreen.h"


#define LED2	13
#define LED1	2

#define STATE_NONE 0


class ScreenManager
{
public:
	ScreenManager();
	void begin(Settings *settings, WiFiManager *wifi_manager, TwoWire *theWire = &Wire);
	void draw(void);
	void run(void);
	void fsInit(void);
	void pushPower(uint16_t volt, uint16_t ampere, uint16_t watt, uint8_t ch);
	void pushInputPower(uint16_t volt, uint16_t ampere, uint16_t watt);
	void disablePower();
	uint8_t* getOnOff(void);
	void checkOnOff();
	void debug();
	bool getShutdown(void);
	void writeSysLED(uint8_t val);
	void writePowerLED(uint8_t val);
	void initLED(void);
	void dimmingLED(uint8_t led);
	VoltageScreen* getVoltageScreen(void);
	SettingScreen* getSettingScreen(void);
	Screen* getActiveScreen(void);
	void setNextScreen(void);
	void getBtnPress(uint8_t idx, uint32_t cur_time, bool long_pressed = false);
	void setTime(uint32_t milisec);
	void setWiFiIconState(void);
	Settings *getSettings();
private:
	TFT_eSPI tft = TFT_eSPI();
	Header *header = nullptr;
	VoltageScreen *voltage_screen = nullptr;
	SettingScreen *setting_screen = nullptr;
	LogoScreen *logo_screen = nullptr;
	Settings *settings = nullptr;
	TwoWire *_wire = nullptr;
	WiFiManager *wifi_manager = nullptr;
	uint32_t dial_time = 0;
	uint32_t cur_time = 0;
	uint8_t onoff[2] = {2, 2};
	bool btn_pressed[5] = {false,};
	uint8_t state_power = 0;
	bool low_input = false;
	uint8_t flag_long_press = false;
	uint32_t count_long_press = 0;
	bool shutdown = false;
	bool updated_wifi_icon = false;
	bool updated_wifi_info = false;

	screen_t screen = LOGO_SCREEN;  // default screen
	Screen *screens[LAST_SCREEN_COUNT] = {};
	bool show_next_screen = false;
};

//ScreenManager screen_manager;
