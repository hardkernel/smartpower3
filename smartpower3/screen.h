#include "FS.h"
#include "SPIFFS.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "channel.h"
#include "header.h"
#include "setting.h"
#include <STPD01.h>
#include <Adafruit_BME280.h>

#define STPD01_CH0 27
#define STPD01_CH1 14

enum screen_mode_t {
	BASE = 0,
	BASE_MOVE,
	BASE_EDIT,
	SETTING,
	SETTING_BL,
	SETTING_FAN,
};

enum state {
	STATE_HEADER = 0,
	STATE_VOLT1,
	STATE_CURRENT1,
	STATE_CURRENT0,
	STATE_VOLT0
};

enum state_setting {
	STATE_FAN = 0,
	STATE_BL,	
	STATE_SETTING,
	STATE_NONE
};

class Screen
{
public:
	Screen();
	void begin(TwoWire *theWire = &Wire);
	void pushPower(uint16_t volt, uint16_t ampere, uint16_t watt, uint8_t ch);
	void pushInputPower(uint16_t volt, uint16_t ampere, uint16_t watt);
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
	bool flag_int = 0;
	void isrSTPD01();
	void initScreen();
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
	static bool _int;
	uint8_t state_power = 0;
	uint8_t old_state_power = 0;
	bool low_input = true;
	fs::FS *fs;
	uint16_t current_limit = 3;
	uint16_t volt_set = 5;
	uint16_t volt_limit = 20;
	bool saved = false;
	bool changed[5] = {false,};
	Adafruit_BME280 bme;
	Adafruit_Sensor *bme_temp = bme.getTemperatureSensor();
	Adafruit_Sensor *bme_humidity = bme.getHumiditySensor();
	uint32_t cnt[2] = {0, 0};
	uint16_t volt, _volt, volt_diff;
	uint8_t flag_on = 0;
	uint8_t flag_off = 1;
	uint16_t time_on = 0;
	uint16_t time_off = 0;
	uint32_t fnd_time = 0;
};
