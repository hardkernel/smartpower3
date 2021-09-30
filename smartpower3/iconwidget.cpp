#include "iconwidget.h"

IconWidget::IconWidget(TFT_eSPI *tft, uint8_t channel)
{
	this->tft = tft;
	this->channel = channel;
}

IconWidget::~IconWidget(void)
{
}

struct icon* IconWidget::init(uint8_t nr_icon, uint16_t x, uint16_t y,
		uint16_t fg_color, uint16_t bg_color)
{
	ic = (struct icon *)malloc(sizeof(struct icon));
	if (ic == NULL) {
		Serial.printf("fnd struct allocation fail!\n\r");
		return NULL;
	}

	ic->x = x;
	ic->y = y;
	ic->bg_color 	= bg_color;
	ic->fg_color 	= fg_color;
	if (nr_icon == 9) {
		ic->width	= 32;
		ic->height	= 32;
	} else if (nr_icon == 8) {
		ic->width	= 16;
		ic->height	= 32;
	} else {
		ic->width	= IMG_OP_WIDTH;
		ic->height	= IMG_OP_HEIGHT;
	}
	ic->nr_icon = nr_icon;

	clearAll();

	return	ic;
}

void IconWidget::clearAll(void)
{
	tft->fillRect(ic->x, ic->y, ic->width, ic->height, ic->bg_color);
}

void IconWidget::update(bool forced, uint8_t channel)
{
	uint16_t	w, h, h_end, i_pos;
	uint8_t		ic_mask;
	const uint8_t	*ic_img;

	if (!ic->refresh && !forced) {
		return;
	}

	ic->refresh = false;

	w     = ic->x;
	h     = ic->y;
	h_end = h + 8;

	ic_img = &IMG_ICON_TABLE[ic->nr_icon][0];

	for (ic_mask = 0x01; h < h_end; h++, ic_mask <<= 1) {
		for (i_pos = 0; i_pos < IMG_OP_WIDTH; i_pos++) {
			if (ic_img[i_pos] & ic_mask)
				tft->drawPixel(w + i_pos, h, ic->fg_color);
		}
		for (i_pos = 0; i_pos < IMG_OP_WIDTH; i_pos++) {
			if (ic_img[i_pos+32] & ic_mask)
				tft->drawPixel(w + i_pos, h+8, ic->fg_color);
		}
	}
}

void IconWidget::setIconColor(uint16_t fg_color, uint16_t bg_color)
{
	if ((ic->fg_color != fg_color) || (ic->bg_color != bg_color))
		ic->refresh = true;
	ic->fg_color = fg_color;
	ic->bg_color = bg_color;
}

void IconWidget::unitsWrite(uint8_t nr_unit)
{
	uint16_t	w, h, h_end, i_pos;
	uint8_t		ic_mask;
	const uint8_t	*ic_img;

	w     = ic->x;
	h     = ic->y;
	h_end = h + 8;

	if (nr_unit == 0)
		ic_img = &IMG_VOLT_DATA2[0];
	else if (nr_unit == 1)
		ic_img = &IMG_AMPERE_DATA2[0];
	else if (nr_unit == 2)
		ic_img = &IMG_WATT_DATA2[0];
	else if (nr_unit == 3)
		ic_img = &IMG_VOLT_DATA3[0];
	else if (nr_unit == 4)
		ic_img = &IMG_AMPERE_DATA3[0];

	for (ic_mask = 0x01; h < h_end; h++, ic_mask <<= 1) {
		for (i_pos = 0; i_pos < ic->width; i_pos++) {
			if (ic_img[i_pos] & ic_mask)
				tft->drawPixel(w + i_pos, h, ic->fg_color);
		}
		for (i_pos = 0; i_pos < ic->width; i_pos++) {
			if (ic_img[i_pos+ic->width] & ic_mask)
				tft->drawPixel(w + i_pos, h+8, ic->fg_color);
		}
		for (i_pos = 0; i_pos < ic->width; i_pos++) {
			if (ic_img[i_pos+ic->width*2] & ic_mask)
				tft->drawPixel(w + i_pos, h+16, ic->fg_color);
		}
		for (i_pos = 0; i_pos < ic->width; i_pos++) {
			if (ic_img[i_pos+ic->width*3] & ic_mask)
				tft->drawPixel(w + i_pos, h+24, ic->fg_color);
		}
	}
}

void IconWidget::inputWrite()
{
	uint16_t	w, h, h_end, i_pos;
	uint8_t		ic_mask;
	const uint8_t	*ic_img;

	w     = ic->x;
	h     = ic->y;
	h_end = h + 8;

	ic_img = &IMG_INPUT_DATA2[0];

	for (ic_mask = 0x01; h < h_end; h++, ic_mask <<= 1) {
		for (i_pos = 0; i_pos < 32; i_pos++) {
			if (ic_img[i_pos] & ic_mask)
				tft->drawPixel(w + i_pos, h, ic->fg_color);
		}
		for (i_pos = 0; i_pos < 32; i_pos++) {
			if (ic_img[i_pos+32] & ic_mask)
				tft->drawPixel(w + i_pos, h+8, ic->fg_color);
		}
		for (i_pos = 0; i_pos < 32; i_pos++) {
			if (ic_img[i_pos+64] & ic_mask)
				tft->drawPixel(w + i_pos, h+16, ic->fg_color);
		}
		for (i_pos = 0; i_pos < 32; i_pos++) {
			if (ic_img[i_pos+96] & ic_mask)
				tft->drawPixel(w + i_pos, h+24, ic->fg_color);
		}
	}
}

void IconWidget::wifiWrite()
{
	uint16_t	w, h, h_end, i_pos;
	uint8_t		ic_mask;
	const uint8_t	*ic_img;

	w     = ic->x;
	h     = ic->y;
	h_end = h + 8;

	ic_img = &IMG_WIFI_DATA[0];

	for (ic_mask = 0x01; h < h_end; h++, ic_mask <<= 1) {
		for (i_pos = 0; i_pos < 32; i_pos++) {
			if (ic_img[i_pos] & ic_mask)
				tft->drawPixel(w + i_pos, h, ic->fg_color);
		}
		for (i_pos = 0; i_pos < 32; i_pos++) {
			if (ic_img[i_pos+32] & ic_mask)
				tft->drawPixel(w + i_pos, h+8, ic->fg_color);
		}
		for (i_pos = 0; i_pos < 32; i_pos++) {
			if (ic_img[i_pos+64] & ic_mask)
				tft->drawPixel(w + i_pos, h+16, ic->fg_color);
		}
		for (i_pos = 0; i_pos < 32; i_pos++) {
			if (ic_img[i_pos+96] & ic_mask)
				tft->drawPixel(w + i_pos, h+24, ic->fg_color);
		}
	}
}

void IconWidget::logWrite()
{
	uint16_t	w, h, h_end, i_pos;
	uint8_t		ic_mask;
	const uint8_t	*ic_img;

	w     = ic->x;
	h     = ic->y;
	h_end = h + 8;

	ic_img = &IMG_LOG_DATA2[0];

	for (ic_mask = 0x01; h < h_end; h++, ic_mask <<= 1) {
		for (i_pos = 0; i_pos < 32; i_pos++) {
			if (ic_img[i_pos] & ic_mask)
				tft->drawPixel(w + i_pos, h, ic->fg_color);
		}
		for (i_pos = 0; i_pos < 32; i_pos++) {
			if (ic_img[i_pos+32] & ic_mask)
				tft->drawPixel(w + i_pos, h+8, ic->fg_color);
		}
		for (i_pos = 0; i_pos < 32; i_pos++) {
			if (ic_img[i_pos+64] & ic_mask)
				tft->drawPixel(w + i_pos, h+16, ic->fg_color);
		}
		for (i_pos = 0; i_pos < 32; i_pos++) {
			if (ic_img[i_pos+96] & ic_mask)
				tft->drawPixel(w + i_pos, h+24, ic->fg_color);
		}
	}
}

void IconWidget::polarityWrite(bool polarity)
{
	uint16_t	w, h, h_end, i_pos;
	uint8_t		ic_mask;
	const uint8_t	*ic_img;

	w     = ic->x;
	h     = ic->y;
	h_end = h + 8;

	if (polarity)
		ic_img = &IMG_POSITIVE_DATA[0];
	else
		ic_img = &IMG_NEGATIVE_DATA[0];

	for (ic_mask = 0x01; h < h_end; h++, ic_mask <<= 1) {
		for (i_pos = 0; i_pos < 32; i_pos++) {
			if (ic_img[i_pos] & ic_mask)
				tft->drawPixel(w + i_pos, h, ic->fg_color);
		}
		for (i_pos = 0; i_pos < 32; i_pos++) {
			if (ic_img[i_pos+32] & ic_mask)
				tft->drawPixel(w + i_pos, h+8, ic->fg_color);
		}
		for (i_pos = 0; i_pos < 32; i_pos++) {
			if (ic_img[i_pos+64] & ic_mask)
				tft->drawPixel(w + i_pos, h+16, ic->fg_color);
		}
		for (i_pos = 0; i_pos < 32; i_pos++) {
			if (ic_img[i_pos+96] & ic_mask)
				tft->drawPixel(w + i_pos, h+24, ic->fg_color);
		}
	}
}

void IconWidget::clearInt(void)
{
	value = false;
}

void IconWidget::setInt(bool val)
{
	if (val) {
		//setIconColor(TFT_RED, BG_ENABLED_INT);
	} else {
		//setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
	}

	value = val;
}

bool IconWidget::getInt(void)
{
	return value;
}
