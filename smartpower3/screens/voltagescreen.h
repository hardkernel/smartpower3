#include <Arduino.h>
#include <TFT_eSPI.h>
#include "channel.h"
#include "header.h"
#include <STPD01.h>
#include "settings.h"
#include "screens/screen.h"

#define LED2	13
#define LED1	2

enum voltage_screen_mode_t {
	VOLTAGE_BASE = 0,
	VOLTAGE_BASE_MOVE,
	VOLTAGE_BASE_EDIT,
};

enum screen_state_base {
	STATE_VOLT1 = 1,
	STATE_CURRENT1,
	STATE_CURRENT0,
	STATE_VOLT0,
	STATE_WIFI,
	STATE_LOGGING
};


class VoltageScreen : public Screen
{
public:
	VoltageScreen();
	VoltageScreen(TFT_eSPI *tft, Header *header, Settings *settings, TwoWire *theWire, WiFiManager *wifi_manager, uint8_t onoff[]);
	~VoltageScreen();
	void pushPower(uint16_t volt, uint16_t ampere, uint16_t watt, uint8_t ch);
	void pushInputPower(uint16_t volt, uint16_t ampere, uint16_t watt);

	virtual void init();
	virtual bool draw(void);
	virtual void select();
	virtual void deSelect(void);

	virtual void onShutdown(void);
	virtual void onWakeup(void);
	virtual void onLeave(void);
	virtual void onEnter(void);
	virtual screen_t getType(void);

	static void isr_stp(void);

	void changeVolt(voltage_screen_mode_t mode);
	bool flag_int[2] = {0,};
	void checkSTPD01InterruptStat();
	void disablePower();
	void setIntFlag(uint8_t channel);
	uint8_t getIntStat(uint8_t channel);
	void countLowVoltage(uint8_t ch=0);
	void clearLowVoltage(uint8_t ch=0);
	void writeSysLED(uint8_t val);
	void writePowerLED(uint8_t val);
	void setWiFiIconState(void);

	Channel* getChannel(uint8_t channel_number);
	bool getEnabledSTPD01(uint8_t which_stpd);
	void setEnabledSTPD01(uint8_t which_stpd, bool enabled);
private:
	voltage_screen_mode_t mode = VOLTAGE_BASE;
	Channel *channel[2];
	TwoWire *_wire;
	void drawBase(void);
	void drawBaseEdit(void);
	void drawBaseMove(void);
	uint8_t state_power = 0;
	bool low_input = false;
	uint16_t current_limit = 3;
	uint16_t volt_set = 5;
	uint32_t fnd_time = 0;
	bool enabled_stpd01[2] = {0,};
	uint8_t int_stat[2] = {0,};
};
