#include "base_icon_widget.h"

BaseIconWidget::BaseIconWidget(TFT_eSPI *tft)
{
	this->tft = tft;
	ic = nullptr;
}

BaseIconWidget::~BaseIconWidget(void)
{
}

struct icon* BaseIconWidget::init(uint16_t x, uint16_t y, uint16_t fg_color,
		uint16_t bg_color)
{
	ic = (struct icon *)malloc(sizeof(struct icon));
	if (ic == NULL) {
		return NULL;
	}

	ic->x = x;
	ic->y = y;
	ic->bg_color = bg_color;
	ic->fg_color = fg_color;
	ic->width = this->iconWidth();
	ic->height = this->iconHeight();

	clearAll();

	return	ic;
}

void BaseIconWidget::draw()
{
	uint16_t w, h, h_end, i_pos;
	uint8_t ic_mask;
	const uint8_t *ic_img = this->iconCharArray();

	w     = ic->x;
	h     = ic->y;
	h_end = h + 8;

	for (ic_mask = 0x01; h < h_end; h++, ic_mask <<= 1) {
		for (i_pos = 0; i_pos < ic->width; i_pos++) {
			if (ic_img[i_pos] & ic_mask)
				tft->drawPixel(w + i_pos, h, ic->fg_color);
		}
		for (i_pos = 0; i_pos < ic->width; i_pos++) {
			if (ic_img[i_pos+ic->width] & ic_mask)
				tft->drawPixel(w + i_pos, h+8, ic->fg_color);
		}
		if (ic->width <= ic->height) {
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
}

// this is done so that the dimensions can be easily redefined in children
const uint16_t BaseIconWidget::iconWidth(void)
{
	return (uint16_t) ICON_DIMENSION_LARGE_SQUARE;
}

const uint16_t BaseIconWidget::iconHeight(void)
{
	return (uint16_t) ICON_DIMENSION_LARGE_SQUARE;
}

const uint8_t* BaseIconWidget::iconCharArray(void)
{
	return NULL;
}

void BaseIconWidget::clearAll(void)
{
	tft->fillRect(ic->x, ic->y, ic->width, ic->height, ic->bg_color);
}

void BaseIconWidget::setIconColor(uint16_t fg_color, uint16_t bg_color)
{
	if ((ic->fg_color != fg_color) || (ic->bg_color != bg_color))
		ic->refresh = true;
	ic->fg_color = fg_color;
	ic->bg_color = bg_color;
}

void BaseIconWidget::select(void)
{
	this->selected = true;
	this->drawOutLines();
}

void BaseIconWidget::deselect(void)
{
	this->selected = false;
	this->clearOutLines();
}

bool BaseIconWidget::getSelected(void)
{
	return this->selected;
}

void BaseIconWidget::drawOutLines(void)
{
	for (int i = 0; i < SELECTION_BORDER_WIDTH; i++) {
		tft->drawRect(
				ic->x-(SELECTION_BORDER_WIDTH-i),
				ic->y-(SELECTION_BORDER_WIDTH-i),
				ic->width+(SELECTION_BORDER_WIDTH*2-i*2),
				ic->height+(SELECTION_BORDER_WIDTH*2-i*2),
				TFT_YELLOW);
	}
}

void BaseIconWidget::clearOutLines(void)
{
	for (int i = 0; i < SELECTION_BORDER_WIDTH; i++) {
		tft->drawRect(
				ic->x-(SELECTION_BORDER_WIDTH-i),
				ic->y-(SELECTION_BORDER_WIDTH-i),
				ic->width+(SELECTION_BORDER_WIDTH*2-i*2),
				ic->height+(SELECTION_BORDER_WIDTH*2-i*2),
				TFT_BLACK);
	}
}
