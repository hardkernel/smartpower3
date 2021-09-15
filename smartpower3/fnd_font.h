//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "fnd_font_32x64.h"
#include "icon.h"
//#include "fnd_font_40x80.h"
//#include "fnd_font_48x96.h"

//------------------------------------------------------------------------------
#define	NUM_OF_FND	4
#define	DOT_POS		1
#define	FG_COLOR	TFT_DARKGREY
#define	BG_COLOR	TFT_BLACK

#define	NUM_OF_ICON 1

//------------------------------------------------------------------------------
#define	FND_CLEAR_NUM	8

//------------------------------------------------------------------------------
const int8_t FONT_IMG_TABLE[16][8][3] = {
	{
		/* FND 0 */
		SEG_ON_A, SEG_ON_B, SEG_ON_C, SEG_ON_D,
		SEG_ON_E, SEG_ON_F, SEG_END
	},{
		/* FND 1 */
		SEG_ON_B, SEG_ON_C, SEG_END,
	},{
		/* FND 2 */
		SEG_ON_A, SEG_ON_B, SEG_ON_D,
		SEG_ON_E, SEG_ON_G, SEG_END,
	},{
		/* FND 3 */
		SEG_ON_A, SEG_ON_B, SEG_ON_C, SEG_ON_D,
		SEG_ON_G, SEG_END,
	},{
		/* FND 4 */
		SEG_ON_B, SEG_ON_C,
		SEG_ON_F, SEG_ON_G, SEG_END,
	},{
		/* FND 5 */
		SEG_ON_A, SEG_ON_C, SEG_ON_D,
		SEG_ON_F, SEG_ON_G, SEG_END,
	},{
		/* FND 6 */
		SEG_ON_A, SEG_ON_C, SEG_ON_D,
		SEG_ON_E, SEG_ON_F, SEG_ON_G, SEG_END,
	},{
		/* FND 7 */
		SEG_ON_A, SEG_ON_B, SEG_ON_C,
		SEG_ON_F, SEG_END,
	},{
		/* FND 8 */
		SEG_ON_A, SEG_ON_B, SEG_ON_C, SEG_ON_D,
		SEG_ON_E, SEG_ON_F, SEG_ON_G, SEG_END,
	},{
		/* FND 9 */
		SEG_ON_A, SEG_ON_B, SEG_ON_C, SEG_ON_D,
		SEG_ON_F, SEG_ON_G, SEG_END,
	},{
		/* FND A */
		SEG_ON_A, SEG_ON_B, SEG_ON_C,
		SEG_ON_E, SEG_ON_F, SEG_ON_G, SEG_END,
	},{
		/* FND b */
		SEG_ON_C, SEG_ON_D,
		SEG_ON_E, SEG_ON_F, SEG_ON_G, SEG_END,
	},{
		/* FND c */
		SEG_ON_D,
		SEG_ON_E, SEG_ON_G, SEG_END,
	},{
		/* FND d */
		SEG_ON_B, SEG_ON_C, SEG_ON_D,
		SEG_ON_E, SEG_ON_G, SEG_END,
	},{
		/* FND E */
		SEG_ON_A, SEG_ON_D,
		SEG_ON_E, SEG_ON_F, SEG_ON_G, SEG_END,
	},{
		/* FND F */
		SEG_ON_A,
		SEG_ON_E, SEG_ON_F, SEG_ON_G, SEG_END,
	},
};

//------------------------------------------------------------------------------

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

	/* num of fnd */
	uint8_t			cnt;

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
