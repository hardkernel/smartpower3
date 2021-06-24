#include "component.h"
#include "fndwidget.h"
#include <STPD01.h>

#define OFFSET_FND 10
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
	Channel(TFT_eSPI *tft, TwoWire *theWire = &Wire, uint8_t channel = 0);
	~Channel(void);
	void drawChannel(void);
	void pushPower(uint16_t volt, uint16_t current, uint16_t watt);
	void pushPowerEdit(uint16_t volt, uint16_t current, uint16_t watt);
	void initScreen(uint16_t width, uint16_t height);
	void initPower(void);
	bool on(void);
	bool off(void);
	void setVolt(float volt_set, uint8_t mode = 0);
	void setCurrentLimit(float val);
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
private:
	bool activated;
	TFT_eSPI *tft;
	uint16_t volt_set = 5000;
	uint16_t _volt_set = 5000;
	uint16_t current_limit = 3000;
	uint16_t _current_limit = 3000;
	FndWidget *volt;
	Component *_volt;
	Component *_current;
	Component *_int_ovp;
	Component *_int_cc;
	Component *_int_otp;
	Component *_int_otw;
	FndWidget *current;
	FndWidget *watt;
	//Component *watt;
	STPD01 *stpd01;
	uint8_t channel;
};
