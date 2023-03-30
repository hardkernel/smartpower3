#include "component.h"
#include "fndwidget.h"
#include "icons/protection_channel_icon_widget.h"
#include "icons/polarity_channel_icon_widget.h"
#include "icons/unit_channel_icon_widget.h"
#include <STPD01.h>

#define OFFSET_FND 5
#define PIN_EN_STPD01_CH0 27
#define PIN_EN_STPD01_CH1 14

#define PIN_INT_CH0	25
#define PIN_INT_CH1	26

enum comp_t {
	VOLT,
	CURRENT,
	WATT
};

class Channel
{
public:
	Channel(TFT_eSPI *tft, TwoWire *theWire=&Wire, uint16_t x=0, uint16_t y=0,
			uint8_t channel=0);
	~Channel(void);
	void drawChannel(bool forced=0);
	void drawVoltSet(bool forced=0);
	void pushPower(uint16_t volt, uint16_t current, uint16_t watt);
	void pushPowerEdit(void);
	void initScreen(uint8_t onoff);
	bool on(void);
	bool off(void);
	void setVolt(float volt_set, uint8_t mode = 0);
	void setCurrentLimit(float val, uint8_t mode = 0);
	void activate(comp_t comp);
	void deActivate(comp_t comp);
	void setCompColor(comp_t comp);
	void clearCompColor(void);
	void editVolt(float volt_set);
	void editCurrentLimit(float val);
	uint16_t getVolt(void);
	uint16_t getCurrentLimit(void);
	uint8_t checkInterrupt(void);
	uint8_t checkInterruptLatch(void);
	uint8_t checkInterruptStat(uint8_t onoff);
	bool isAvailableSTPD01();
	uint8_t getIntStatus(void);
	uint8_t getIntMask(void);
	uint16_t getValueVolt();
	uint16_t getValueCurrent();
	uint16_t getValueWatt();
	void write(uint8_t addr, uint8_t reg);
	void setHide();
	void clearHide();
	void disabled();
	void enable();
	void isr(uint8_t onoff);
	void setIntFlag(void);
	void drawInterrupt(void);
	void drawPolarity(void);
	void drawUnits(bool onoff);
	void clearLowVoltage();
	void countLowVoltage();
	void clearDebug(void);

private:
	bool activated = false;
	TFT_eSPI *tft;
	uint16_t volt_set = 5000;
	uint16_t _volt_set = 5000;
	uint16_t current_limit = 500;
	uint16_t _current_limit = 500;
	FndWidget *volt;
	FndWidget *_volt;
	FndWidget *_current;
#ifdef DEBUG_STPD01
	Component *stpd;
	Component *lowV;
#endif
	Component *debug_intr[8];
	FndWidget *current;
	FndWidget *watt;
	ProtectionChannelIconWidget *icon_op, *icon_sp, *icon_tp, *icon_tw, *icon_ip;
	PolarityChannelIconWidget *icon_positive, *icon_negative;
	UnitChannelIconWidget *icon_volt, *icon_ampere, *icon_watt;
	STPD01 *stpd01;
	uint8_t channel;
	uint16_t x, y;
	uint8_t hide = 0;
	uint16_t moving_avg[5] = {0,};
	uint8_t cnt_mavg= 0;
	uint8_t en_stpd01[2] = {PIN_EN_STPD01_CH0, PIN_EN_STPD01_CH1};
#ifdef USE_SINGLE_IRQ_STPD01
	uint8_t int_stpd01[2] = {PIN_INT_CH0, PIN_INT_CH0};
#else
	uint8_t int_stpd01[2] = {PIN_INT_CH0, PIN_INT_CH1};
#endif
	bool flag_int = 0;
	uint8_t latch = 0;
	uint8_t stat = 0;
	uint16_t low_volt = 0;
	uint8_t count_intr[8] = {0,};
	uint8_t flag_clear_debug = 0;
};
