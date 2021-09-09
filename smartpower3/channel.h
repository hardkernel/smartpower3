#include "component.h"
#include "fndwidget.h"
#include <STPD01.h>

#define OFFSET_FND 5
#define Y_VOLT FONT_HEIGHT*0 + OFFSET_FND*0
#define Y_CURRENT FONT_HEIGHT*1 + OFFSET_FND*1
#define Y_WATT FONT_HEIGHT*2 + OFFSET_FND*2
#define PIN_EN_STPD01_CH0 27
#define PIN_EN_STPD01_CH1 14

#define PIN_INT_CH0	25
#define PIN_INT_CH1	26

enum comp_t {
	HEADER,
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
	void deleteScreen(void);
	void drawChannel(bool forced=0);
	void drawVoltSet(bool forced=0);
	void pushPower(uint16_t volt, uint16_t current, uint16_t watt);
	void pushPowerEdit(void);
	void initScreen();
	void initPower(void);
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
	void monitorInterrupt();
	uint8_t checkInterrupt(void);
	void clearInterruptUI(void);
	bool isAvailableSTPD01();
	uint8_t getIntStatus(void);
	uint8_t getIntMask(void);
	uint16_t getValueVolt();
	uint16_t getValueCurrent();
	uint16_t getValueWatt();
	void write(uint8_t addr, uint8_t reg);
	void setHide();
	void clearHide();
	void initSTPD01();
	void disabled();
	void enable();
	void isr(void);
	void setIntFlag(void);
private:
	bool activated;
	TFT_eSPI *tft;
	uint16_t volt_set = 5000;
	uint16_t _volt_set = 5000;
	uint16_t current_limit = 500;
	uint16_t _current_limit = 500;
	FndWidget *volt;
	Component *_volt;
	Component *_current;
	Component *int_stat[8];
	Component *int_latch[8];
	Component *int_mask[8];
	Component *stpd;
	FndWidget *current;
	FndWidget *watt;
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
};
