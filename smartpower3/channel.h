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
	float _volt_set = 5.0;
	float ampere_limit = 2.0;
	float _ampere_limit = 2.0;
	Component *volt;
	Component *_volt;
	Component *_ampere;
	Component *ampere;
	Component *watt;
public:
	Channel(TFT_eSPI *tft, uint16_t width, uint16_t height);
	~Channel(void);
	void drawPower(void);
	void pushPower(float volt, float ampere, float watt);
	void pushPowerEdit(float volt, float ampere, float watt);
	void init(void);
	void powerOn(bool onoff);
	void powerOn();
	void setVolt(float volt_set);
	void setAmpereLimit(float ampere_limit);
	void activate(comp_t comp);
	void deActivate(comp_t comp);
	void setCompColor(comp_t comp);
	void clearCompColor(void);
	void editVolt(float volt_set);
	void editAmpereLimit(float ampere_limit);
	bool getOnOff(void);
};

