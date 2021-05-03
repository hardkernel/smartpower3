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

class Component
{
private:
	uint8_t mode = 0;
	bool activated;
	uint16_t width;
	uint16_t height;
	uint16_t x;
	uint16_t y;
	uint8_t font;
	TFT_eSprite *img;
public:
	Component(TFT_eSPI *tft, uint16_t width, uint16_t height, uint8_t font);
	~Component(void);

	void init(uint16_t fg_color, uint16_t bg_color, uint8_t size, uint8_t align);
	void draw(float val, uint16_t x, uint16_t y);
	void draw(String s, uint16_t x, uint16_t y);
	void draw(float val);
	void setCoordinate(uint16_t x, uint16_t y);
};
