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
		uint16_t x, uint16_t y, uint16_t fg_color, uint16_t bg_color, enum FND_FONT_SIZE f_size, uint32_t div)
{
	free(f);
	f = (struct fnd *)malloc(sizeof(struct fnd));
	if (f == NULL) {
		Serial.print(F("fnd struct allocation fail!\n\r"));
		return NULL;
	}

	f->dd = (struct display_data *)malloc(cnt * sizeof(struct display_data));
	if (f->dd == NULL) {
		Serial.print(F("fnd_init display data buffer allocation fail!\n\r"));
		free(f);
		return NULL;
	}

	f->f_info = (struct font_info *)malloc(sizeof(struct font_info));
	if (f->f_info == NULL) {
		Serial.print(F("fnd_init fnd frame buffer allocation fail!\n\r"));
		free(f->f_info);
		free(f->dd);
		free(f);
		return NULL;
	}
	fnd_font_init(f->f_info, f_size);

	f->fb = (uint16_t *)malloc(f->f_info->w * f->f_info->h * sizeof(uint16_t));
	if (f->fb == NULL) {
		Serial.print(F("fnd_init fnd frame buffer allocation fail!\n\r"));
		free(f->f_info);
		free(f->dd);
		free(f);
		return NULL;
	}

	f->x = x;
	f->y = y;
	f->bg_color = bg_color;
	f->fg_color = fg_color;
	f->cnt = cnt;
	f->dot_en = false;
	f->dot_on = false;
	f->rbo = rbo;
	f->f_size = f_size;
	f->div = div;
	width = cnt * f->f_info->w;
	height = f->f_info->h;

	if (dot_pos && (dot_pos < f->cnt)) {
		f->dot_pos = f->cnt - dot_pos;
		f->dot_x_offset = f->dot_pos * f->f_info->w;
		f->dot_en = true;
	}

	for (uint32_t i = 0; i < f->cnt; i++) {
		f->dd[i].value  = 0;
		f->dd[i].update = true;
	}
	fnd_clear_all();

	return f;
}


void FndWidget::fnd_num_write(uint16_t f_pos)
{
	uint16_t w_offset;

	w_offset = f->f_info->w * f_pos;

	if (f->dot_en && (f_pos >= f->dot_pos))
		w_offset += f->f_info->d_width;

	tft->pushImage(f->x + w_offset, f->y,
			f->f_info->w, f->f_info->h, &f->fb[0]);
}

void FndWidget::fnd_clear_all(void)
{
	uint16_t f_width, w, h;

	f_width = f->cnt * f->f_info->w;
	if (f->dot_en) {
		f_width  += f->f_info->d_width;
		f->dot_on = false;
	}

	/* FND fb clear */
	for (h = 0; h < f->f_info->h; h++) {
		for (w = 0; w < f->f_info->w; w++)
			f->fb[w + h * f->f_info->w] = f->bg_color;
	}
	tft->fillRect(f->x, f->y, f_width, f->f_info->h, f->bg_color);
}
void FndWidget::fnd_dot_write(void)
{
	uint16_t w, h, h_end, i_pos;
	uint8_t f_mask;
	const uint8_t *f_img;

	w     = f->x + f->dot_x_offset;
	h     = f->y + f->f_info->d_y_offset;
	h_end = h    + f->f_info->d_height;
	f_img = f->f_info->d_data;

	for (f_mask = 0x01; h < h_end; h++, f_mask <<= 1) {
		for (i_pos = 0; i_pos < f->f_info->d_width; i_pos++) {
			if (f_img[i_pos] & f_mask)
				tft->drawPixel(w + i_pos, h, f->fg_color);
		}
	}
}

struct font_table *fnd_font_table(struct fnd *f, uint8_t f_value)
{
	struct font_table *p = NULL;

	switch(f->f_size) {
		case FND_FONT_16x32:
			p = (struct font_table *)&FONT_IMG_TABLE_16x32[f_value][0][0];
			break;
		case FND_FONT_32x64:
			p = (struct font_table *)&FONT_IMG_TABLE_32x64[f_value][0][0];
			break;
		case FND_FONT_40x80:
			p = (struct font_table *)&FONT_IMG_TABLE_40x80[f_value][0][0];
			break;
		case FND_FONT_48x96:
			p = (struct font_table *)&FONT_IMG_TABLE_48x96[f_value][0][0];
			break;
		default:
			break;
	}
	return p;
}

void FndWidget::fnd_fb_write(int8_t f_pos, int8_t f_value, int16_t color)
{
	struct font_table *f_tbl;
	const uint8_t *f_img;
	uint16_t f_img_h, f_img_w, h, h_end, w, w_end, i_pos, i_offset;
	uint8_t f_mask;

	f_tbl = fnd_font_table(f, f_value);

	while((f_tbl->x_offset != -1) && (f_tbl->y_offset != -1)) {
		f_img    = f_tbl->img_sel ? f->f_info->h_data   : f->f_info->v_data;
		f_img_h  = f_tbl->img_sel ? f->f_info->h_height : f->f_info->v_height;
		f_img_w  = f_tbl->img_sel ? f->f_info->h_width  : f->f_info->v_width;

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
					f->fb[w + h * f->f_info->w] = color;
			}
		}
		f_tbl++;
	}
}

void FndWidget::fnd_dd_clear()
{
	for (int i = 0; i < f->cnt; i++) {
		f->dd[i].value  = -1;
		f->dd[i].update = true;
	}
}

void FndWidget::fnd_update(bool forced)
{
	uint16_t i, fnd_pos;
	bool rbo = f->rbo;
	uint16_t value = this->value;
	uint32_t fnd_data = 0;

	if (forced) {
		for (i = 0; i < f->cnt; i++) {
			fnd_pos = f->cnt -i -1;
			f->dd[fnd_pos].update = true;
		}
		f->dot_en = true;
		f->dot_on = false;
	} else if (f->refresh) {
		f->refresh = false;
	} else if (value == old_value) {
		return;
	}

	if (!forced) {
		if (value != 0) {
			if (this->set_current_limit && diff) {
				if (diff < value) {
					setFGColor(TFT_YELLOW);
				} else {
					setFGColor(TFT_RED);
				}
			}

			if (this->set_monitor_voltage && diff) {
				float diff_abs = abs(diff - value);
				if (diff_abs/value > 0.05) {
					setFGColor(TFT_YELLOW);
				} else {
					setFGColor(TFT_RED);
				}
			}
		}
	}

	old_value = value;

	if (f->div)
		fnd_data = (value > 0) ? (value / f->div) : 0;
	else
		fnd_data = value;

	for (i = 0; i < f->cnt; i++) {
		fnd_pos = f->cnt -i -1;
		if (value) {
			if (f->dd[fnd_pos].value != (fnd_data % 10)) {
				f->dd[fnd_pos].value  = fnd_data % 10;
				f->dd[fnd_pos].update = true;
			}
			fnd_data /= 10;
		} else {
			if (f->dd[fnd_pos].value) {
				f->dd[fnd_pos].value  = 0;
				f->dd[fnd_pos].update = true;
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

void FndWidget::fnd_font_init(struct font_info *f_info, enum FND_FONT_SIZE font)
{
    switch(font) {
        case FND_FONT_16x32:
            f_info->w = FONT_WIDTH_16x32;
            f_info->h = FONT_HEIGHT_16x32;
            f_info->v_width = IMG_V_WIDTH_16x32;
            f_info->h_width = IMG_H_WIDTH_16x32;
            f_info->d_width = IMG_DOT_WIDTH_16x32;
            f_info->v_height = IMG_V_HEIGHT_16x32;
            f_info->h_height = IMG_H_HEIGHT_16x32;
            f_info->d_height = IMG_DOT_HEIGHT_16x32;
            f_info->d_y_offset = IMG_DOT_YOFFSET_16x32;
            f_info->v_data = &IMG_V_DATA_16x32[0];
            f_info->h_data = &IMG_H_DATA_16x32[0];
            f_info->d_data = &IMG_DOT_DATA_16x32[0];
        break;
        case FND_FONT_40x80:
            f_info->w = FONT_WIDTH_40x80;
            f_info->h = FONT_HEIGHT_40x80;
            f_info->v_width = IMG_V_WIDTH_40x80;
            f_info->h_width = IMG_H_WIDTH_40x80;
            f_info->d_width = IMG_DOT_WIDTH_40x80;
            f_info->v_height = IMG_V_HEIGHT_40x80;
            f_info->h_height = IMG_H_HEIGHT_40x80;
            f_info->d_height = IMG_DOT_HEIGHT_40x80;
            f_info->d_y_offset = IMG_DOT_YOFFSET_40x80;
            f_info->v_data = &IMG_V_DATA_40x80[0];
            f_info->h_data = &IMG_H_DATA_40x80[0];
            f_info->d_data = &IMG_DOT_DATA_40x80[0];
        break;
        case FND_FONT_32x64:
            f_info->w = FONT_WIDTH_32x64;
            f_info->h = FONT_HEIGHT_32x64;
            f_info->v_width = IMG_V_WIDTH_32x64;
            f_info->h_width = IMG_H_WIDTH_32x64;
            f_info->d_width = IMG_DOT_WIDTH_32x64;
            f_info->v_height = IMG_V_HEIGHT_32x64;
            f_info->h_height = IMG_H_HEIGHT_32x64;
            f_info->d_height = IMG_DOT_HEIGHT_32x64;
            f_info->d_y_offset = IMG_DOT_YOFFSET_32x64;
            f_info->v_data = &IMG_V_DATA_32x64[0];
            f_info->h_data = &IMG_H_DATA_32x64[0];
            f_info->d_data = &IMG_DOT_DATA_32x64[0];
        break;
        case FND_FONT_48x96:
            f_info->w = FONT_WIDTH_48x96;
            f_info->h = FONT_HEIGHT_48x96;
            f_info->v_width = IMG_V_WIDTH_48x96;
            f_info->h_width = IMG_H_WIDTH_48x96;
            f_info->d_width = IMG_DOT_WIDTH_48x96;
            f_info->v_height = IMG_V_HEIGHT_48x96;
            f_info->h_height = IMG_H_HEIGHT_48x96;
            f_info->d_height = IMG_DOT_HEIGHT_48x96;
            f_info->d_y_offset = IMG_DOT_YOFFSET_48x96;
            f_info->v_data = &IMG_V_DATA_48x96[0];
            f_info->h_data = &IMG_H_DATA_48x96[0];
            f_info->d_data = &IMG_DOT_DATA_48x96[0];
        default:
        break;
    }
}

void FndWidget::init(uint16_t fg_color, uint16_t bg_color, uint8_t size, uint8_t align)
{
#if 0
	img->setColorDepth(8);
	img->fillSprite(BG_COLOR);
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

void FndWidget::setFGColor(uint16_t fg_color)
{
	if (f->fg_color != fg_color) {
		f->fg_color = fg_color;
		f->dot_on = false;
		fnd_dd_clear();
	}
}

void FndWidget::setCoordinate(uint16_t x, uint16_t y)
{
	f->x = x;
	f->y = y;
}

void FndWidget::drawOutLines(void)
{
	for (int i = 0; i < 2; i++) {
		tft->drawRect(f->x-(i+2), f->y-(i+1), width+6+(i*2), height+(3+i*2), TFT_YELLOW);
	}
}

void FndWidget::clearOutLines(void)
{
	for (int i = 0; i < 2; i++) {
		tft->drawRect(f->x-(i+2), f->y-(i+1), width+6+(i*2), height+(3+i*2), BG_COLOR);
	}
}

void FndWidget::pushValue(uint16_t value, uint16_t diff)
{
	this->value = value;
	this->diff = diff;
}

void FndWidget::pushValue(uint16_t value)
{
	this->value = value;
}

uint16_t FndWidget::getValue()
{
	return this->value;
}

void FndWidget::select(void)
{
	drawOutLines();
}

void FndWidget::deSelect(void)
{
	clearOutLines();
}

void FndWidget::setCurrentLimit(void)
{
	this->set_current_limit = true;
}

void FndWidget::setMonitorVoltage(void)
{
	this->set_monitor_voltage = true;
}
