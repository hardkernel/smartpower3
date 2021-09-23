//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "fnd_font_16x32.h"
#include "fnd_font_32x64.h"
#include "fnd_font_40x80.h"
#include "fnd_font_48x96.h"
#include "icon.h"

//------------------------------------------------------------------------------
#define	NUM_OF_FND	4
#define	DOT_POS		1
#define	FG_COLOR	TFT_DARKGREY
#define	BG_COLOR	TFT_BLACK

#define	NUM_OF_ICON 1

//------------------------------------------------------------------------------
#define	FND_CLEAR_NUM	8

enum FND_FONT_SIZE {
	FND_FONT_16x32,
	FND_FONT_32x64,
	FND_FONT_40x80,
	FND_FONT_48x96,
};

struct font_info {
	uint32_t	w, h;
	uint32_t	v_width, v_height;
	uint32_t	h_width, h_height;
	uint32_t	d_width, d_height;
	const uint8_t	*v_data, *h_data, *d_data;
	uint16_t	d_y_offset;
};

struct font_table {
	int8_t		x_offset, y_offset;
	int8_t		img_sel;
};

struct display_data {
	int8_t		value;
	bool		update;
};

struct fnd {
	uint16_t		*fb;
	struct display_data	*dd;
	struct font_info	*f_info;

	enum FND_FONT_SIZE	f_size;

	/* num of fnd */
	uint8_t			cnt;

	uint32_t div;

	/* fnd position */
	uint16_t		x, y;

	/* dot enable */
	bool			dot_en, dot_on;
	uint8_t			dot_pos;
	uint16_t		dot_x_offset;

	// FND ripple blanking control
	bool			rbo;
	uint16_t		bg_color, fg_color;
	bool			refresh;

	bool			off;
};

struct icon {
	/* fnd position */
	uint16_t		x, y;
	uint16_t	width, height;

	// FND ripple blanking control
	uint16_t		bg_color, fg_color;
	bool			refresh;
	uint8_t nr_icon;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
