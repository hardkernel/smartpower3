#include "component.h"

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
	uint16_t x;
	uint16_t y;
	TFT_eSPI *tft;
	float volt_set = 5.0;
public:
	Channel(TFT_eSPI *tft, uint16_t width, uint16_t height);
	~Channel(void);
	Component *volt;
	Component *_volt;
	Component *ampere;
	Component *watt;
	void drawPower(void);
	void pushPower(float volt, float ampere, float watt);
	void pushPowerEdit(float volt, float ampere, float watt);
	void init(void);
	void drawOutLines(uint8_t idx);
	void clearOutLines(uint8_t idx);
	void powerOn(bool onoff);
	void powerOn();
	void setVolt(float volt_set);
	void activate(comp_t comp);
	void deActivate(comp_t comp);
};

