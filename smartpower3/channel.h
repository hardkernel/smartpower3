#include "component.h"

class Channel
{
private:
	bool activated;
	uint16_t x;
	uint16_t y;
	Component *volt;
	Component *ampere;
	Component *watt;
	TFT_eSPI *tft;
public:
	Channel(TFT_eSPI *tft, uint16_t width, uint16_t height);
	~Channel(void);
	void drawVoltage(float watt);
	void drawAmpere(float watt);
	void drawWatt(float watt);
	void init(void);
	void drawOutLines(uint8_t idx);
	void clearOutLines(uint8_t idx);
	void activate(uint8_t idx);
};

