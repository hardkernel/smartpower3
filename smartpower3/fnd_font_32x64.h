//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef	_FND_FONT_32x64__H_

#define	_FND_FONT_32x64__H_
//------------------------------------------------------------------------------
#define	FONT_WIDTH_32x64		32
#define	FONT_HEIGHT_32x64		64

//------------------------------------------------------------------------------
/* Vertical Segment Image */
//------------------------------------------------------------------------------
#define	IMG_V_32x64				0
#define	IMG_V_WIDTH_32x64		5
#define	IMG_V_HEIGHT_32x64		26

const uint8_t	IMG_V_DATA_32x64[] = {
	0xFC, 0xFE, 0xFF, 0xFE, 0xFC,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x01, 0x03, 0x01, 0x00
};

//------------------------------------------------------------------------------
/* Horizontal Segment Image */
//------------------------------------------------------------------------------
#define	IMG_H_32x64				1
#define	IMG_H_WIDTH_32x64		20
#define	IMG_H_HEIGHT_32x64		5

const uint8_t	IMG_H_DATA_32x64[] = {
	0x04, 0x0E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
	0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0E, 0x04
};

//------------------------------------------------------------------------------
/* Dot Segment Image */
//------------------------------------------------------------------------------
#define	IMG_DOT_32x64			2
#define	IMG_DOT_WIDTH_32x64	4
#define	IMG_DOT_HEIGHT_32x64	4

#define	IMG_DOT_YOFFSET_32x64	58

const uint8_t	IMG_DOT_DATA_32x64[] = {
	0x0F, 0x0F, 0x0F, 0x0F
};

//------------------------------------------------------------------------------
/* Segment display data table (x offsset, y offset, img select */
//------------------------------------------------------------------------------
#define SEG_ON_A_32x64 	{  6,  3, IMG_H_32x64 	}
#define SEG_ON_B_32x64 	{ 25,  6, IMG_V_32x64 	}
#define SEG_ON_C_32x64 	{ 25, 33, IMG_V_32x64	}
#define SEG_ON_D_32x64 	{  6, 57, IMG_H_32x64	}
#define SEG_ON_E_32x64 	{  2, 33, IMG_V_32x64	}
#define SEG_ON_F_32x64 	{  2,  6, IMG_V_32x64	}
#define SEG_ON_G_32x64 	{  6, 30, IMG_H_32x64	}
#define SEG_END_32x64		{ -1, -1, -1	}

//------------------------------------------------------------------------------
const int8_t FONT_IMG_TABLE_32x64[16][8][3] = {
	{
		/* FND 0 */
		SEG_ON_A_32x64, SEG_ON_B_32x64, SEG_ON_C_32x64, SEG_ON_D_32x64,
		SEG_ON_E_32x64, SEG_ON_F_32x64, SEG_END_32x64
	},{
		/* FND 1 */
		SEG_ON_B_32x64, SEG_ON_C_32x64, SEG_END_32x64,
	},{
		/* FND 2 */
		SEG_ON_A_32x64, SEG_ON_B_32x64, SEG_ON_D_32x64,
		SEG_ON_E_32x64, SEG_ON_G_32x64, SEG_END_32x64,
	},{
		/* FND 3 */
		SEG_ON_A_32x64, SEG_ON_B_32x64, SEG_ON_C_32x64, SEG_ON_D_32x64,
		SEG_ON_G_32x64, SEG_END_32x64,
	},{
		/* FND 4 */
		SEG_ON_B_32x64, SEG_ON_C_32x64,
		SEG_ON_F_32x64, SEG_ON_G_32x64, SEG_END_32x64,
	},{
		/* FND 5 */
		SEG_ON_A_32x64, SEG_ON_C_32x64, SEG_ON_D_32x64,
		SEG_ON_F_32x64, SEG_ON_G_32x64, SEG_END_32x64,
	},{
		/* FND 6 */
		SEG_ON_A_32x64, SEG_ON_C_32x64, SEG_ON_D_32x64,
		SEG_ON_E_32x64, SEG_ON_F_32x64, SEG_ON_G_32x64, SEG_END_32x64,
	},{
		/* FND 7 */
		SEG_ON_A_32x64, SEG_ON_B_32x64, SEG_ON_C_32x64,
		SEG_ON_F_32x64, SEG_END_32x64,
	},{
		/* FND 8 */
		SEG_ON_A_32x64, SEG_ON_B_32x64, SEG_ON_C_32x64, SEG_ON_D_32x64,
		SEG_ON_E_32x64, SEG_ON_F_32x64, SEG_ON_G_32x64, SEG_END_32x64,
	},{
		/* FND 9 */
		SEG_ON_A_32x64, SEG_ON_B_32x64, SEG_ON_C_32x64, SEG_ON_D_32x64,
		SEG_ON_F_32x64, SEG_ON_G_32x64, SEG_END_32x64,
	},
};

//------------------------------------------------------------------------------
#endif	// #ifndef	_FND_FONT_32x64__H_

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
