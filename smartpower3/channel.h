#include "component.h"
#include "fndwidget.h"
#include <STPD01.h>

#define OFFSET_FND 5
#define Y_VOLT FONT_HEIGHT*0 + OFFSET_FND*0
#define Y_CURRENT FONT_HEIGHT*1 + OFFSET_FND*1
#define Y_WATT FONT_HEIGHT*2 + OFFSET_FND*2
#define PIN_EN_STPD01_CH0 27
#define PIN_EN_STPD01_CH1 14

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
	void pushPower(uint16_t volt, uint16_t current, uint16_t watt);
	void pushPowerEdit(uint16_t volt, uint16_t current, uint16_t watt);
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
	void checkInterrupt(void);
	void clearInterruptUI(void);
	uint8_t test(void);
	void monitorSTPD01();
	uint8_t getIntStatus(void);
	uint8_t getIntMask(void);
	uint16_t getValueVolt();
	uint16_t getValueCurrent();
	uint16_t getValueWatt();
	void write(uint8_t addr, uint8_t reg);
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
	//Component *watt;
	STPD01 *stpd01;
	uint8_t channel;
	uint16_t x, y;
};
