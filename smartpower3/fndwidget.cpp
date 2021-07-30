#include "fndwidget.h"

FndWidget::FndWidget(TFT_eSPI *tft) : Component(tft)
{
	this->tft = tft;
	value = 0;
}

FndWidget::~FndWidget(void)
{
}

struct fnd* FndWidget::fnd_init(uint8_t cnt, uint8_t dot_pos, bool rbo,
	uint16_t x, uint16_t y, uint16_t fg_color, uint16_t bg_color)
{
	uint32_t	i;

	f = (struct fnd *)malloc(sizeof(struct fnd));
	if (f == NULL) {
		Serial.printf("fnd struct allocation fail!\n\r");
		return NULL;
	}

	f->dd = (struct display_data *)malloc(cnt * sizeof(struct display_data));
	if (f->dd == NULL) {
		Serial.printf("fnd_init display data buffer allocation fail!\n\r");
		free(f);
		return NULL;
	}

	f->fb = (uint16_t *)malloc(FONT_WIDTH * FONT_HEIGHT * sizeof(uint16_t));
	if (f->fb == NULL) {
		Serial.printf("fnd_init fnd frame buffer allocation fail!\n\r");
		free(f->dd);
		free(f);
		return NULL;
	}

	this->x = x;
	this->y = y;
	f->bg_color 	= bg_color;
	f->fg_color 	= fg_color;
	f->cnt 		= cnt;
	f->dot_en	= false;
	f->dot_on 	= false;
	f->rbo		= rbo;
	width = f->cnt * FONT_WIDTH;
	height = FONT_HEIGHT;

	if (dot_pos && (dot_pos < f->cnt)) {
		f->dot_pos 	= f->cnt - dot_pos;
		f->dot_x_offset = f->dot_pos * FONT_WIDTH;
		f->dot_en = true;
	}

	for (i = 0; i < f->cnt; i++) {
		f->dd[i].value  = 0;
		f->dd[i].update = true;
	}
	fnd_clear_all();

	return	f;
}

void FndWidget::fnd_num_write(uint16_t f_pos)
{
	uint16_t	w_offset;

	w_offset = FONT_WIDTH * f_pos;

	if (f->dot_en && (f_pos >= f->dot_pos))
		w_offset += IMG_DOT_WIDTH;

	tft->pushImage(x + w_offset, y,
			FONT_WIDTH, FONT_HEIGHT, &f->fb[0]);
}

void FndWidget::fnd_clear_all(void)
{
	uint16_t	f_width, w, h;

	f_width = f->cnt * FONT_WIDTH;
	if (f->dot_en) {
		f_width  += IMG_DOT_WIDTH;
		f->dot_on = false;
	}

	/* FND fb clear */
	for (h = 0; h < FONT_HEIGHT; h++) {
		for (w = 0; w < FONT_WIDTH; w++)
			f->fb[w + h * FONT_WIDTH] = f->bg_color;
	}
	tft->fillRect(x, y, f_width, FONT_HEIGHT, f->bg_color);
}

void FndWidget::fnd_dot_write(void)
{
	uint16_t	w, h, h_end, i_pos;
	uint8_t		f_mask;
	const uint8_t	*f_img;

	w     = x + f->dot_x_offset;
	h     = y + IMG_DOT_YOFFSET;
	h_end = h    + IMG_DOT_HEIGHT;
	f_img = &IMG_DOT_DATA[0];

	for (f_mask = 0x01; h < h_end; h++, f_mask <<= 1) {
		for (i_pos = 0; i_pos < IMG_DOT_WIDTH; i_pos++) {
			if (f_img[i_pos] & f_mask)
				tft->drawPixel(w + i_pos, h, f->fg_color);
		}
	}
}

void FndWidget::fnd_fb_write(int8_t f_pos, int8_t f_value, int16_t color)
{
	struct font_table	*f_tbl;
	const uint8_t		*f_img;
	uint16_t		f_img_h, f_img_w, f_width;
	uint8_t			f_mask;

	uint16_t	h, h_end, w, w_end, w_offset;
	uint16_t	i_pos, i_offset;

	f_tbl = (struct font_table *)&FONT_IMG_TABLE[f_value][0][0];

	while((f_tbl->x_offset != -1) && (f_tbl->y_offset != -1)) {
		f_img    = f_tbl->img_sel ? IMG_H_DATA   : IMG_V_DATA;
		f_img_h  = f_tbl->img_sel ? IMG_H_HEIGHT : IMG_V_HEIGHT;
		f_img_w  = f_tbl->img_sel ? IMG_H_WIDTH  : IMG_V_WIDTH;

		h_end    = f_img_h + f_tbl->y_offset;
		w_end    = f_img_w + f_tbl->x_offset;
		f_mask   = 0x01;
		i_offset = 0;

		for (h = f_tbl->y_offset; h < h_end; h++, f_mask <<= 1) {

			if (!f_mask) {
				f_mask = 0x01;
				i_offset++;
			}
			i_pos = i_offset * f_img_w;

			for (w = f_tbl->x_offset; w < w_end; w++, i_pos++) {
				if (f_img[i_pos] & f_mask)
					f->fb[w + h * FONT_WIDTH] = color;
			}
		}
		f_tbl++;
	}
}

void FndWidget::fnd_dd_clear()
{
	int i;
	for (i = 0; i < f->cnt; i++) {
		f->dd[i].value  = -1;
		f->dd[i].update = true;
	}
}

void FndWidget::fnd_update(bool forced)
{
	uint16_t	i, pos;
	bool		rbo = f->rbo;
	uint16_t value = this->value;

	if (forced) {
		for (i = 0; i < f->cnt; i++) {
			pos = f->cnt -i -1;
			f->dd[pos].update = true;
		}
	} else if (f->refresh) {
		f->refresh = false;
	} else if (value == old_value) {
		return;
	}
	old_value = value;

	for (i = 0; i < f->cnt; i++) {
		pos = f->cnt -i -1;
		if (value) {
			if (f->dd[pos].value != (value % 10)) {
				f->dd[pos].value  = value % 10;
				f->dd[pos].update = true;
			}
			value /= 10;
		} else {
			if (f->dd[pos].value) {
				f->dd[pos].value  = 0;
				f->dd[pos].update = true;
			}
		}
	}

	for (i = 0; i < f->cnt; i++) {
		fnd_fb_write (i, FND_CLEAR_NUM,  f->bg_color);
		if (f->dd[i].update) {
			f->dd[i].update = false;

			if ((i + 1 < f->cnt) && (f->dd[i +1].value == 0))
				f->dd[i +1].update = true;

			if (rbo && (f->dd[i].value == 0)) {
				fnd_num_write(i);
				if ( f->dot_en && ((i + 1) < f->dot_pos))
					continue;
				if (!f->dot_en && ((i + 1) < f->cnt))
					continue;
			}
			rbo = false;
			fnd_fb_write (i, f->dd[i].value,  f->fg_color);
			fnd_num_write(i);
		}
	}

	if (f->dot_en && !f->dot_on) {
		fnd_dot_write();
		f->dot_on = true;
	}
}

void FndWidget::init(uint16_t fg_color, uint16_t bg_color, uint8_t size, uint8_t align)
{
#if 0
	img->setColorDepth(8);
	img->fillSprite(TFT_BLACK);
	img->createSprite(width, height);
	img->setTextColor(fg_color, bg_color);
	img->setTextSize(size);
	img->setTextDatum(align);
#endif
}

void FndWidget::setTextColor(uint16_t fg_color, uint16_t bg_color)
{
	if ((f->fg_color != fg_color) || (f->bg_color != bg_color))
		f->refresh = true;
	f->fg_color = fg_color;
	f->bg_color = bg_color;
	f->dot_on = false;
	fnd_dd_clear();
}

void FndWidget::setCoordinate(uint16_t x, uint16_t y)
{
	this->x = x;
	this->y = y;
}

void FndWidget::drawOutLines(void)
{
	for (int i = 0; i < 3; i++) {
		tft->drawRect(x-(i+3), y-(i+3), width+30+(i*2), height+(6+i*2), TFT_YELLOW);
	}
}

void FndWidget::clearOutLines(void)
{
	for (int i = 0; i < 3; i++) {
		tft->drawRect(x-(i+3), y-(i+3), width+30+(i*2), height+(6+i*2), TFT_DARKGREY);
	}
}

void FndWidget::draw(String s)
{
#if 0
	img->drawString(s, 0, 0, font);
	//img->pushSprite(x, y);
	delay(WAIT);
#endif
}

void FndWidget::draw(void)
{
#if 0
	if (value == value_old)
		return;
	value_old = value;
	if (value < 10)
		img->drawString("0" + String(value, 2), width, 0, font);
	else
		img->drawString(String(value, 2), width, 0, font);
	img->pushSprite(x, y);
	delay(WAIT);
#endif
}

void FndWidget::pushValue(uint16_t value)
{
	this->value = value/10;
}

uint16_t FndWidget::getValue()
{
	return this->value;
}

void FndWidget::activate(void)
{
	drawOutLines();
}

void FndWidget::deActivate(void)
{
	clearOutLines();
}
