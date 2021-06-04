//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#ifndef	_FND_FONT_48x96__H_

#define	_FND_FONT_48x96__H_

//------------------------------------------------------------------------------
#define	FONT_WIDTH	48
#define	FONT_HEIGHT	96

//------------------------------------------------------------------------------
/* Vertical Segment Image */
//------------------------------------------------------------------------------
#define	IMG_V		0
#define	IMG_V_WIDTH	11
#define	IMG_V_HEIGHT	40

const uint8_t	IMG_V_DATA[] = {
	0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07
};

//------------------------------------------------------------------------------
/* Horizontal Segment Image */
//------------------------------------------------------------------------------
#define	IMG_H		1
#define	IMG_H_WIDTH	30
#define	IMG_H_HEIGHT	11

const uint8_t	IMG_H_DATA[] = {
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
#define	IMG_DOT		2
#define	IMG_DOT_WIDTH	10
#define	IMG_DOT_HEIGHT	10

#define	IMG_DOT_YOFFSET	85

const uint8_t	IMG_DOT_DATA[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
};

//------------------------------------------------------------------------------
/* Segment display data table (x offsset, y offset, img select */
//------------------------------------------------------------------------------
#define SEG_ON_A 	{  9,  2, IMG_H	}
#define SEG_ON_B 	{ 35,  8, IMG_V	}
#define SEG_ON_C 	{ 35, 49, IMG_V	}
#define SEG_ON_D 	{  9, 84, IMG_H	}
#define SEG_ON_E 	{  2, 49, IMG_V	}
#define SEG_ON_F 	{  2,  8, IMG_V	}
#define SEG_ON_G 	{  9, 43, IMG_H	}
#define SEG_END 	{ -1, -1, -1	}

//------------------------------------------------------------------------------
#endif	// #ifndef	_FND_FONT_48x96__H_

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
