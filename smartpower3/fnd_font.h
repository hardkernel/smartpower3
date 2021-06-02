//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define	FONT_WIDTH	32
#define	FONT_HEIGHT	64

//------------------------------------------------------------------------------
/* Vertical Segment Image */
//------------------------------------------------------------------------------
#define	IMG_V		0
#define	IMG_V_WIDTH	5
#define	IMG_V_HEIGHT	26

#define	NUM_OF_FND	4
#define	DOT_POS		1
#define	FG_COLOR	TFT_RED
#define	BG_COLOR	TFT_BLACK
//uint16_t bg_color = TFT_DARKGREY;

const uint8_t	IMG_V_DATA[] = {
	0xFC, 0xFE, 0xFF, 0xFE, 0xFC,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x01, 0x03, 0x01, 0x00
};

//------------------------------------------------------------------------------
/* Horizontal Segment Image */
//------------------------------------------------------------------------------
#define	IMG_H		1
#define	IMG_H_WIDTH	20
#define	IMG_H_HEIGHT	5

const uint8_t	IMG_H_DATA[] = {
	0x04, 0x0E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
	0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0E, 0x04
};

//------------------------------------------------------------------------------
/* Dot Segment Image */
//------------------------------------------------------------------------------
#define	IMG_DOT		2
#define	IMG_DOT_WIDTH	4
#define	IMG_DOT_HEIGHT	4

#define	IMG_DOT_YOFFSET	58

const uint8_t	IMG_DOT_DATA[] = {
	0x0F, 0x0F, 0x0F, 0x0F
};

//------------------------------------------------------------------------------
/* Segment display data table (x offsset, y offset, img select */
//------------------------------------------------------------------------------
#define SEG_ON_A 	{  6,  3, IMG_H	}
#define SEG_ON_B 	{ 25,  6, IMG_V	}
#define SEG_ON_C 	{ 25, 33, IMG_V	}
#define SEG_ON_D 	{  6, 57, IMG_H	}
#define SEG_ON_E 	{  2, 33, IMG_V	}
#define SEG_ON_F 	{  2,  6, IMG_V	}
#define SEG_ON_G 	{  6, 30, IMG_H	}
#define SEG_END 	{ -1, -1, -1	}

const int8_t FONT_IMG_TABLE[16][8][3] = {
	{
		/* FND 0 */
		SEG_ON_A, SEG_ON_B, SEG_ON_C, SEG_ON_D,
		SEG_ON_E, SEG_ON_F, SEG_END, SEG_END
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
#define	FND_CLEAR_NUM	8

struct font_table {
	int8_t		x_offset, y_offset;
	int8_t		img_sel;
};

struct display_data {
	int8_t		value;
	bool		update;
};

struct fnd {
	uint16_t			*fb;
	struct display_data	*dd;

	/* num of fnd */
	uint8_t			cnt;

	/* dot enable */
	bool			dot_en, dot_on;
	uint8_t			dot_pos;
	uint16_t		dot_x_offset;

	// FND ripple blanking control
	bool			rbo;
	uint16_t		bg_color, fg_color;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
