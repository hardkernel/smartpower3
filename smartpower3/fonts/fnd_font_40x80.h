//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef	_FND_FONT_40x80__H_

#define	_FND_FONT_40x80__H_

//------------------------------------------------------------------------------
#define	FONT_WIDTH_40x80		40
#define	FONT_HEIGHT_40x80		80

//------------------------------------------------------------------------------
/* Vertical Segment Image */
//------------------------------------------------------------------------------
#define	IMG_V_40x80				0
#define	IMG_V_WIDTH_40x80		9
#define	IMG_V_HEIGHT_40x80		32

const uint8_t	IMG_V_DATA_40x80[] = {
	0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFE, 0xFC, 0xF8, 0xF0,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F
};

//------------------------------------------------------------------------------
/* Horizontal Segment Image */
//------------------------------------------------------------------------------
#define	IMG_H_40x80				1
#define	IMG_H_WIDTH_40x80		24
#define	IMG_H_HEIGHT_40x80		9

const uint8_t	IMG_H_DATA_40x80[] = {
	0x10, 0x38, 0x7C, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x7C, 0x38, 0x10,

	0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00
};

//------------------------------------------------------------------------------
/* Dot Segment Image */
//------------------------------------------------------------------------------
#define	IMG_DOT_40x80			2
#define	IMG_DOT_WIDTH_40x80	8
#define	IMG_DOT_HEIGHT_40x80	8

#define	IMG_DOT_YOFFSET_40x80	70

const uint8_t	IMG_DOT_DATA_40x80[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

//------------------------------------------------------------------------------
/* Segment display data table (x offsset, y offset, img select */
//------------------------------------------------------------------------------
#define SEG_ON_A_40x80 	{  8,  3, IMG_H_40x80	}
#define SEG_ON_B_40x80 	{ 29,  8, IMG_V_40x80	}
#define SEG_ON_C_40x80 	{ 29, 41, IMG_V_40x80	}
#define SEG_ON_D_40x80 	{  8, 69, IMG_H_40x80	}
#define SEG_ON_E_40x80 	{  2, 41, IMG_V_40x80	}
#define SEG_ON_F_40x80 	{  2,  8, IMG_V_40x80	}
#define SEG_ON_G_40x80 	{  8, 36, IMG_H_40x80	}
#define SEG_END_40x80 		{ -1, -1, -1	}

//------------------------------------------------------------------------------
const int8_t FONT_IMG_TABLE_40x80[16][8][3] = {
	{
		/* FND 0 */
		SEG_ON_A_40x80, SEG_ON_B_40x80, SEG_ON_C_40x80, SEG_ON_D_40x80,
		SEG_ON_E_40x80, SEG_ON_F_40x80, SEG_END_40x80
	},{
		/* FND 1 */
		SEG_ON_B_40x80, SEG_ON_C_40x80, SEG_END_40x80,
	},{
		/* FND 2 */
		SEG_ON_A_40x80, SEG_ON_B_40x80, SEG_ON_D_40x80,
		SEG_ON_E_40x80, SEG_ON_G_40x80, SEG_END_40x80,
	},{
		/* FND 3 */
		SEG_ON_A_40x80, SEG_ON_B_40x80, SEG_ON_C_40x80, SEG_ON_D_40x80,
		SEG_ON_G_40x80, SEG_END_40x80,
	},{
		/* FND 4 */
		SEG_ON_B_40x80, SEG_ON_C_40x80,
		SEG_ON_F_40x80, SEG_ON_G_40x80, SEG_END_40x80,
	},{
		/* FND 5 */
		SEG_ON_A_40x80, SEG_ON_C_40x80, SEG_ON_D_40x80,
		SEG_ON_F_40x80, SEG_ON_G_40x80, SEG_END_40x80,
	},{
		/* FND 6 */
		SEG_ON_A_40x80, SEG_ON_C_40x80, SEG_ON_D_40x80,
		SEG_ON_E_40x80, SEG_ON_F_40x80, SEG_ON_G_40x80, SEG_END_40x80,
	},{
		/* FND 7 */
		SEG_ON_A_40x80, SEG_ON_B_40x80, SEG_ON_C_40x80,
		SEG_ON_F_40x80, SEG_END_40x80,
	},{
		/* FND 8 */
		SEG_ON_A_40x80, SEG_ON_B_40x80, SEG_ON_C_40x80, SEG_ON_D_40x80,
		SEG_ON_E_40x80, SEG_ON_F_40x80, SEG_ON_G_40x80, SEG_END_40x80,
	},{
		/* FND 9 */
		SEG_ON_A_40x80, SEG_ON_B_40x80, SEG_ON_C_40x80, SEG_ON_D_40x80,
		SEG_ON_F_40x80, SEG_ON_G_40x80, SEG_END_40x80,
	},
};

//------------------------------------------------------------------------------
#endif	// #ifndef	_FND_FONT_40x80__H_

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
