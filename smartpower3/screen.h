#include "FS.h"
#include "SPIFFS.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "channel.h"
#include "header.h"
#include "setting.h"
#include <STPD01.h>

#define LED2	13
#define LED1	2

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
	STATE_VOLT1 = 0,
	STATE_CURRENT1,
	STATE_CURRENT0,
	STATE_VOLT0,
};

enum state_setting {
	STATE_LOG = 0,
	STATE_FAN,
	STATE_BL,
	STATE_NONE
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
	void countDial(int8_t mode_count, int8_t direct, uint8_t step, uint32_t milisec);
	void deActivate();
	void deActivateSetting();
	void getBtnPress(uint8_t idx, uint32_t cur_time);
	void setTime(uint32_t milisec);
	void clearBtnEvent(void);
	void checkOnOff();
	static void isr_stp(void);
	void listDir(const char * dirname, uint8_t levels);
	void readFile(const char * path);
	void drawBmp(const char *filename, int16_t x, int16_t y);
	uint16_t read16(fs::File &f);
	uint32_t read32(fs::File &f);
	void changeVolt(screen_mode_t mode);
	void fsInit(void);
	void setSysParam(const char *key, String value);
	void setSysParam(const char *key, float value);
	bool isFirstBoot();
	bool flag_int[2] = {0,};
	void isrSTPD01();
	void initScreen();
	void debug();
	void disablePower();
	void setIntFlag(uint8_t channel);
	uint16_t getLogInterval(void);
	uint8_t getIntStat(uint8_t channel);
	bool checkAttachBtn(uint8_t pin);
	void disableBtn(void);
	void enableBtn(void);
	void countLowVoltage(uint8_t ch=0);
	void clearLowVoltage(uint8_t ch=0);
	bool getShutdown(void);
	void writeSysLED(uint8_t val);
	void writePowerLED(uint8_t val);
	void initLED(void);
	void dimmingLED(uint8_t led);
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
	bool btn_pressed[5] = {false,};
	int16_t dial_cnt = 0;
	int16_t dial_cnt_old;
	int8_t dial_direct;
	uint8_t dial_state;
	uint8_t dial_step;
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
	uint8_t int_stat[2] = {0,};
	bool flag_attach[4] = {0,};
	uint8_t flag_long_press = false;
	uint32_t count_long_press = 0;
	bool shutdown = false;
};
