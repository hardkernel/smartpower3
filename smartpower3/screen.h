#include <Arduino.h>
#include <TFT_eSPI.h>
#include "component.h"

#define WAIT 1
#define IWIDTH  138
#define IHEIGHT 48
#define W_HEADER 320
#define H_HEADER 40

#define W_CH0 20
#define OFFSET_CH0 15
#define H_VOLT (H_HEADER + OFFSET_CH0)
#define H_AMPERE (H_VOLT + IHEIGHT + OFFSET_CH0)
#define H_WATT (H_AMPERE + IHEIGHT + OFFSET_CH0)
#define FONT_SEVEN_SEGMENT 7

class Channel
{
private:
	bool activated;
	uint16_t x;
	uint16_t y;
	Component *volt;
	Component *ampere;
	Component *watt;
	void init(void);
public:
	Channel(TFT_eSPI *tft, uint16_t width, uint16_t height);
	~Channel(void);
	void drawVoltage(float watt);
	void drawAmpere(float watt);
	void drawWatt(float watt);
};

class Screen
{
private:
	uint8_t mode = NULL;
	TFT_eSPI tft = TFT_eSPI();
	TFT_eSprite tft_ampere = TFT_eSprite(&tft);
	TFT_eSprite tft_watt = TFT_eSprite(&tft);
	TFT_eSprite tft_volt = TFT_eSprite(&tft);

	TFT_eSprite tft_header = TFT_eSprite(&tft);

	void initSprite(TFT_eSprite *sprite, uint16_t width, uint16_t height,
			uint16_t fg_color, uint16_t bg_color, uint8_t align, uint8_t size);
	void initBaseMode(void);

	float cnt = 0;
	Channel *channel[2];

public:
	Screen();
	void setMode(uint8_t val);
	uint8_t getMode(void);
	void drawHeader(String s);
	void drawScreen(void);
	void drawVoltage(float volt, uint8_t ch);
	void drawAmpere(float ampere, uint8_t ch);
	void drawWatt(float watt, uint8_t ch);
	void changeMode(uint8_t val);
	Component *header;
};
