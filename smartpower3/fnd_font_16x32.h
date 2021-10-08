//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef	_FND_FONT_16x32__H_

#define	_FND_FONT_16x32__H_

//------------------------------------------------------------------------------
#define	FONT_WIDTH_16x32		16
#define	FONT_HEIGHT_16x32		32

//------------------------------------------------------------------------------
/* Vertical Segment Image */
//------------------------------------------------------------------------------
#define	IMG_V_16x32 			0
#define	IMG_V_WIDTH_16x32		3
#define	IMG_V_HEIGHT_16x32		12

const uint8_t	IMG_V_DATA_16x32[] = {
	0xFE, 0xFF, 0xFE, 0x07, 0x0F, 0x07
};

//------------------------------------------------------------------------------
/* Horizontal Segment Image */
//------------------------------------------------------------------------------
#define	IMG_H_16x32				1
#define	IMG_H_WIDTH_16x32		8
#define	IMG_H_HEIGHT_16x32		3

const uint8_t	IMG_H_DATA_16x32[] = {
	0x02, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x02
};

//------------------------------------------------------------------------------
/* Dot Segment Image */
//------------------------------------------------------------------------------
#define	IMG_DOT_16x32			2
#define	IMG_DOT_WIDTH_16x32	2
#define	IMG_DOT_HEIGHT_16x32 	2

#define	IMG_DOT_YOFFSET_16x32	29

const uint8_t	IMG_DOT_DATA_16x32[] = {
	0x03, 0x03
};

//------------------------------------------------------------------------------
/* Segment display data table (x offsset, y offset, img select */
//------------------------------------------------------------------------------
#define SEG_ON_A_16x32 	{  4,  2, IMG_H_16x32	}
#define SEG_ON_B_16x32 	{ 12,  4, IMG_V_16x32	}
#define SEG_ON_C_16x32 	{ 12, 17, IMG_V_16x32	}
#define SEG_ON_D_16x32 	{  4, 28, IMG_H_16x32	}
#define SEG_ON_E_16x32 	{  1, 17, IMG_V_16x32	}
#define SEG_ON_F_16x32 	{  1,  4, IMG_V_16x32	}
#define SEG_ON_G_16x32 	{  4, 15, IMG_H_16x32	}
#define SEG_END_16x32	 	{ -1, -1, -1	}

//------------------------------------------------------------------------------
const int8_t FONT_IMG_TABLE_16x32[16][8][3] = {
	{
		/* FND 0 */
		SEG_ON_A_16x32, SEG_ON_B_16x32, SEG_ON_C_16x32, SEG_ON_D_16x32,
		SEG_ON_E_16x32, SEG_ON_F_16x32, SEG_END_16x32
	},{
		/* FND 1 */
		SEG_ON_B_16x32, SEG_ON_C_16x32, SEG_END_16x32,
	},{
		/* FND 2 */
		SEG_ON_A_16x32, SEG_ON_B_16x32, SEG_ON_D_16x32,
		SEG_ON_E_16x32, SEG_ON_G_16x32, SEG_END_16x32,
	},{
		/* FND 3 */
		SEG_ON_A_16x32, SEG_ON_B_16x32, SEG_ON_C_16x32, SEG_ON_D_16x32,
		SEG_ON_G_16x32, SEG_END_16x32,
	},{
		/* FND 4 */
		SEG_ON_B_16x32, SEG_ON_C_16x32,
		SEG_ON_F_16x32, SEG_ON_G_16x32, SEG_END_16x32,
	},{
		/* FND 5 */
		SEG_ON_A_16x32, SEG_ON_C_16x32, SEG_ON_D_16x32,
		SEG_ON_F_16x32, SEG_ON_G_16x32, SEG_END_16x32,
	},{
		/* FND 6 */
		SEG_ON_A_16x32, SEG_ON_C_16x32, SEG_ON_D_16x32,
		SEG_ON_E_16x32, SEG_ON_F_16x32, SEG_ON_G_16x32, SEG_END_16x32,
	},{
		/* FND 7 */
		SEG_ON_A_16x32, SEG_ON_B_16x32, SEG_ON_C_16x32,
		SEG_ON_F_16x32, SEG_END_16x32,
	},{
		/* FND 8 */
		SEG_ON_A_16x32, SEG_ON_B_16x32, SEG_ON_C_16x32, SEG_ON_D_16x32,
		SEG_ON_E_16x32, SEG_ON_F_16x32, SEG_ON_G_16x32, SEG_END_16x32,
	},{
		/* FND 9 */
		SEG_ON_A_16x32, SEG_ON_B_16x32, SEG_ON_C_16x32, SEG_ON_D_16x32,
		SEG_ON_F_16x32, SEG_ON_G_16x32, SEG_END_16x32,
	},
};

//------------------------------------------------------------------------------
#endif	// #ifndef	_FND_FONT_16x32__H_

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
