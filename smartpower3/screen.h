#pragma once

#include "FS.h"
#include "SPIFFS.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "channel.h"
#include "header.h"
#include "setting.h"
#include <STPD01.h>
#include <Adafruit_BME280.h>

enum screen_mode_t {
	BASE = 0,
	BASE_MOVE,
	BASE_EDIT,
	SETTING,
	SETTING_BL,
	SETTING_FAN,
	SETTING_LOG,
};

enum state {
	STATE_HEADER = 0,
	STATE_VOLT1,
	STATE_CURRENT1,
	STATE_CURRENT0,
	STATE_VOLT0
};

enum state_setting {
	STATE_LOG = 0,
	STATE_FAN,
	STATE_BL,
	STATE_SETTING,
	STATE_NONE
};

enum RemoteSetupMode {
	RS_NONE = 0,
	RS_ONOFF,
	RS_VOLTAGE,
	RS_CURRENT_LIMIT,
	RS_BACKLIGHT_LEVEL,
	RS_FAN_SPEED,
	RS_LOG_INTERVAL
};

struct RemoteSetupData {
	RemoteSetupMode mode;
	uint8_t targetChannel;
	uint16_t voltage;
	uint16_t currentLimit;
	uint8_t backlightLevel;
	uint8_t fanSpeed;
	uint8_t logInterval;
};

class Screen
{
public:
	Screen();
	void begin(TwoWire *theWire = &Wire);
	void pushPower(uint16_t volt, uint16_t ampere, uint16_t watt, uint8_t ch);
	void pushInputPower(uint16_t volt, uint16_t ampere, uint16_t watt);
	int8_t* getOnOff(void);
	void run(void);
	void drawScreen(void);
	void activate();
	void activate_setting();
	void countDial(int8_t mode_count, bool direct, uint32_t milisec);
	void deActivate();
	void deActivateSetting();
	void getBtnPress(uint8_t idx, uint32_t cur_time);
	void setTime(uint32_t milisec);
	void clearBtnEvent(void);
	void checkOnOff();
	static void isr_stp(void);
	void listDir(const char * dirname, uint8_t levels);
	void readFile(const char * path);
	void changeVolt(screen_mode_t mode);
	void fsInit(void);
	void setSysParam(char *key, String value);
	void setSysParam(char *key, float value);
	bool isFirstBoot();
	bool flag_int[2] = {0,};
	void isrSTPD01();
	void initScreen();
	void debug();
	void disablePower();
	void setIntFlag(uint8_t channel);
	uint16_t getLogInterval(void);
	uint16_t getVoltSet(uint8_t channel);
	uint16_t getCurrentLimitSet(uint8_t channel);
	static void remoteSwitchChannelOnoff(uint8_t channel);
	static void remoteSetVoltage(uint8_t channel, float volt);
	static void remoteSetCurrentLimit(uint8_t channel, float current);
	static void remoteSetSettings(state_setting mode, uint16_t value);
private:
	TFT_eSPI tft = TFT_eSPI();
	screen_mode_t mode = BASE;
	Channel *channel[2];
	Header *header;
	Setting *setting;
	TwoWire *_wire;
	uint8_t activated = 0;
	uint32_t dial_time = 0;
	uint32_t cur_time = 0;
	uint32_t task_time = 0;
	uint32_t time_print[2] = {0, 0};
	int8_t onoff[2] = {2, 2};
	bool btn_pressed[4] = {false,};
	int8_t dial_cnt = 0;
	int8_t dial_cnt_old;
	bool dial_direct;
	uint8_t dial_state;
	void drawBase(void);
	void drawBaseEdit(void);
	void drawBaseMove(void);
	void drawSetting(void);
	void drawSettingBL(void);
	void drawSettingFAN(void);
	void drawSettingLOG(void);
	static bool _int;
	uint8_t state_power = 0;
	uint8_t old_state_power = 0;
	bool low_input = false;
	fs::FS *fs;
	uint16_t current_limit = 3;
	uint16_t volt_set = 5;
	uint16_t volt_limit = 20;
	bool saved = false;
	bool changed[5] = {false,};
	uint32_t cnt[2] = {0, 0};
	uint16_t volt, _volt, volt_diff;
	uint8_t flag_on = 0;
	uint8_t flag_off = 1;
	uint16_t time_on = 0;
	uint16_t time_off = 0;
	uint32_t fnd_time = 0;
	bool enabled_stpd01[2] = {0,};
	void remoteSetup();
	static bool remoteSetupRequested;
	static RemoteSetupData remoteSetupData;
};
