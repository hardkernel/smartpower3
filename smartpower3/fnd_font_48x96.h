//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef	_FND_FONT_48x96__H_

#define	_FND_FONT_48x96__H_

//------------------------------------------------------------------------------
#define	FONT_WIDTH_48x96		48
#define	FONT_HEIGHT_48x96		96

//------------------------------------------------------------------------------
/* Vertical Segment Image */
//------------------------------------------------------------------------------
#define	IMG_V_48x96				0
#define	IMG_V_WIDTH_48x96		11
#define	IMG_V_HEIGHT_48x96		40

const uint8_t	IMG_V_DATA_48x96[] = {
	0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07
};

//------------------------------------------------------------------------------
/* Horizontal Segment Image */
//------------------------------------------------------------------------------
#define	IMG_H_48x96				1
#define	IMG_H_WIDTH_48x96		30
#define	IMG_H_HEIGHT_48x96		11

const uint8_t	IMG_H_DATA_48x96[] = {
	0x20, 0x70, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xF8, 0x70, 0x20,

	0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00
};

//------------------------------------------------------------------------------
/* Dot Segment Image */
//------------------------------------------------------------------------------
#define	IMG_DOT_48x96			2
#define	IMG_DOT_WIDTH_48x96	10
#define	IMG_DOT_HEIGHT_48x96	10

#define	IMG_DOT_YOFFSET_48x96	85

const uint8_t	IMG_DOT_DATA_48x96[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
};

//------------------------------------------------------------------------------
/* Segment display data table (x offsset, y offset, img select */
//------------------------------------------------------------------------------
#define SEG_ON_A_48x96 	{  9,  2, IMG_H_48x96 	}
#define SEG_ON_B_48x96 	{ 35,  8, IMG_V_48x96 	}
#define SEG_ON_C_48x96 	{ 35, 49, IMG_V_48x96 	}
#define SEG_ON_D_48x96 	{  9, 84, IMG_H_48x96 	}
#define SEG_ON_E_48x96 	{  2, 49, IMG_V_48x96 	}
#define SEG_ON_F_48x96 	{  2,  8, IMG_V_48x96	}
#define SEG_ON_G_48x96 	{  9, 43, IMG_H_48x96	}
#define SEG_END_48x96	 	{ -1, -1, -1	}

//------------------------------------------------------------------------------
const int8_t FONT_IMG_TABLE_48x96[16][8][3] = {
	{
		/* FND 0 */
		SEG_ON_A_48x96, SEG_ON_B_48x96, SEG_ON_C_48x96, SEG_ON_D_48x96,
		SEG_ON_E_48x96, SEG_ON_F_48x96, SEG_END_48x96
	},{
		/* FND 1 */
		SEG_ON_B_48x96, SEG_ON_C_48x96, SEG_END_48x96,
	},{
		/* FND 2 */
		SEG_ON_A_48x96, SEG_ON_B_48x96, SEG_ON_D_48x96,
		SEG_ON_E_48x96, SEG_ON_G_48x96, SEG_END_48x96,
	},{
		/* FND 3 */
		SEG_ON_A_48x96, SEG_ON_B_48x96, SEG_ON_C_48x96, SEG_ON_D_48x96,
		SEG_ON_G_48x96, SEG_END_48x96,
	},{
		/* FND 4 */
		SEG_ON_B_48x96, SEG_ON_C_48x96,
		SEG_ON_F_48x96, SEG_ON_G_48x96, SEG_END_48x96,
	},{
		/* FND 5 */
		SEG_ON_A_48x96, SEG_ON_C_48x96, SEG_ON_D_48x96,
		SEG_ON_F_48x96, SEG_ON_G_48x96, SEG_END_48x96,
	},{
		/* FND 6 */
		SEG_ON_A_48x96, SEG_ON_C_48x96, SEG_ON_D_48x96,
		SEG_ON_E_48x96, SEG_ON_F_48x96, SEG_ON_G_48x96, SEG_END_48x96,
	},{
		/* FND 7 */
		SEG_ON_A_48x96, SEG_ON_B_48x96, SEG_ON_C_48x96,
		SEG_ON_F_48x96, SEG_END_48x96,
	},{
		/* FND 8 */
		SEG_ON_A_48x96, SEG_ON_B_48x96, SEG_ON_C_48x96, SEG_ON_D_48x96,
		SEG_ON_E_48x96, SEG_ON_F_48x96, SEG_ON_G_48x96, SEG_END_48x96,
	},{
		/* FND 9 */
		SEG_ON_A_48x96, SEG_ON_B_48x96, SEG_ON_C_48x96, SEG_ON_D_48x96,
		SEG_ON_F_48x96, SEG_ON_G_48x96, SEG_END_48x96,
	},
};

//------------------------------------------------------------------------------
#endif	// #ifndef	_FND_FONT_48x96__H_

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
