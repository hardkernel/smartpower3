#include <Arduino.h>
#include <TFT_eSPI.h>

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

class Channel
{
};

class Screen
{
public:
	Screen();
	void setMode(uint8_t val);
	uint8_t getMode(void);
	void drawHeader(String s);
	void drawScreen(void);
	void drawVoltage(float volt);
	void drawAmpere(float ampere);
	void drawWatt(float watt);
	void changeMode(uint8_t val);
	void initInstances(void);

private:
	uint8_t mode = NULL;
	TFT_eSPI tft = TFT_eSPI();
	Channel ch0;
	Channel ch1;
	TFT_eSprite tft_ampere = TFT_eSprite(&tft);
	TFT_eSprite tft_watt = TFT_eSprite(&tft);
	TFT_eSprite tft_volt = TFT_eSprite(&tft);

	TFT_eSprite tft_header = TFT_eSprite(&tft);
	void initSprite(TFT_eSprite *sprite, uint16_t width, uint16_t height,
			uint16_t fg_color, uint16_t bg_color, uint8_t align, uint8_t size);
	void initMode0(void);
};

