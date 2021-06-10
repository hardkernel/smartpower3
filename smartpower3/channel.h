#include "component.h"
#include "fndwidget.h"

#define OFFSET_FND 10
#define Y_VOLT FONT_HEIGHT*0 + OFFSET_FND*0
#define Y_AMPERE FONT_HEIGHT*1 + OFFSET_FND*1
#define Y_WATT FONT_HEIGHT*2 + OFFSET_FND*2

enum comp_t {
	HEADER,
	VOLT,
	AMPERE,
	WATT
};

class Channel
{
private:
	bool activated;
	bool onoff;
	TFT_eSPI *tft;
	uint16_t volt_set = 5000;
	uint16_t _volt_set = 5000;
	uint16_t current_limit = 3000;
	uint16_t _current_limit = 3000;
	FndWidget *volt;
	Component *_volt;
	Component *_current;
	FndWidget *ampere;
	FndWidget *watt;
public:
	Channel(TFT_eSPI *tft);
	~Channel(void);
	void drawPower(void);
	void pushPower(uint16_t volt, uint16_t ampere, uint16_t watt);
	void pushPowerEdit(float volt, float ampere, float watt);
	void init(uint16_t width, uint16_t height);
	void powerOn(bool onoff);
	void powerOn();
	void setVolt(float volt_set);
	void setCurrentLimit(float val);
	void activate(comp_t comp);
	void deActivate(comp_t comp);
	void setCompColor(comp_t comp);
	void clearCompColor(void);
	void editVolt(float volt_set);
	void editCurrentLimit(float val);
	bool getOnOff(void);
	uint16_t getVolt(void);
	uint16_t getCurrentLimit(void);
};

