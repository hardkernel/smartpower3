#include <Arduino.h>
#include <TFT_eSPI.h>

#define WAIT 1
#define W_SEG 140
#define H_SEG 48
#define OFFSET_SEG 45
#define OFFSET_CH 15

#define W_HEADER 230
#define H_HEADER 40

#define W_CH0 20

#define FONT_SEVEN_SEGMENT 7
#define OFFSET_X 0
#define OFFSET_Y H_HEADER

class Component
{
private:
	uint8_t mode = 0;
	uint16_t width;
	uint16_t height;
	uint16_t x;
	uint16_t y;
	uint8_t font;
	TFT_eSprite *img;
	TFT_eSPI *tft;
	float value;
	float value_old;
	void drawOutLines(void);
	void clearOutLines(void);
	bool activated = false;
public:
	Component(TFT_eSPI *tft, uint16_t width, uint16_t height, uint8_t font);
	~Component(void);

	void init(uint16_t fg_color, uint16_t bg_color, uint8_t size, uint8_t align);
	void draw(void);
	void draw(String s);
	void setCoordinate(uint16_t x, uint16_t y);
	void pushValue(float value);
	void activate(void);
	void deActivate(void);
};
