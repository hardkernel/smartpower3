//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef	_FND_FONT_40x80__H_

#define	_FND_FONT_40x80__H_

//------------------------------------------------------------------------------
#define	FONT_WIDTH		40
#define	FONT_HEIGHT	80

//------------------------------------------------------------------------------
/* Vertical Segment Image */
//------------------------------------------------------------------------------
#define	IMG_V		0
#define	IMG_V_WIDTH	9
#define	IMG_V_HEIGHT	32

const uint8_t	IMG_V_DATA[] = {
	0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFE, 0xFC, 0xF8, 0xF0,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F
};

//------------------------------------------------------------------------------
/* Horizontal Segment Image */
//------------------------------------------------------------------------------
#define	IMG_H		1
#define	IMG_H_WIDTH	24
#define	IMG_H_HEIGHT	9

const uint8_t	IMG_H_DATA[] = {
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
#define	IMG_DOT		2
#define	IMG_DOT_WIDTH	8
#define	IMG_DOT_HEIGHT	8

#define	IMG_DOT_YOFFSET	70

const uint8_t	IMG_DOT_DATA[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

//------------------------------------------------------------------------------
/* Segment display data table (x offsset, y offset, img select */
//------------------------------------------------------------------------------
#define SEG_ON_A 	{  8,  3, IMG_H	}
#define SEG_ON_B 	{ 29,  8, IMG_V	}
#define SEG_ON_C 	{ 29, 41, IMG_V	}
#define SEG_ON_D 	{  8, 69, IMG_H	}
#define SEG_ON_E 	{  2, 41, IMG_V	}
#define SEG_ON_F 	{  2,  8, IMG_V	}
#define SEG_ON_G 	{  8, 36, IMG_H	}
#define SEG_END 	{ -1, -1, -1	}

//------------------------------------------------------------------------------
#endif	// #ifndef	_FND_FONT_40x80__H_

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
