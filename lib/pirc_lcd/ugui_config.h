#ifndef __UGUI_CONFIG_H
#define __UGUI_CONFIG_H

/* -------------------------------------------------------------------------------- */
/* -- CONFIG SECTION                                                             -- */
/* -------------------------------------------------------------------------------- */
#include <avr/io.h>
#include <avr/pgmspace.h>

//#define USE_MULTITASKING    

/* Enable color mode */
//#define USE_COLOR_RGB888   // RGB = 0xFF,0xFF,0xFF
#define USE_COLOR_RGB565   // RGB = 0bRRRRRGGGGGGBBBBB 

/* Enable needed fonts here */
//#define  USE_RFONT_4X6	// 576 B
//#define  USE_RFONT_5X8	// 768 B
//#define  USE_RFONT_5X12	// 1152 B
//#define  USE_RFONT_6X8	// 768 B
//#define  USE_RFONT_6X10	// 960 B
//#define  USE_RFONT_7X12	// 1152 B
//#define  USE_RFONT_8X8	// 768 B
#define  USE_RFONT_8X12		// 1152 B
//#define  USE_RFONT_8X14	// 1344 B
//#define  USE_RFONT_10X16	// 3072 B
//#define  USE_RFONT_12X16	// 3072 B
//#define  USE_RFONT_12X20	// 3840 B
//#define  USE_RFONT_16X26	// 4992 B
//#define  USE_RFONT_22X36	// 10368 B
//#define  USE_RFONT_24X40	// 11520 B
//#define  USE_RFONT_32X53	// 20352 Bytes

//#define  USE_FONT_4X6		// 1536 B
//#define  USE_FONT_5X8		// 2048 B
//#define  USE_FONT_5X12	// 3072 B
//#define  USE_FONT_6X8		// 2048 B
//#define  USE_FONT_6X10	// 2560 B
//#define  USE_FONT_7X12	// 3072 B
#define  USE_FONT_8X8		// 2048 B
//#define  USE_FONT_8X12_CYRILLIC
//#define  USE_FONT_8X12	// 3072 B
//#define  USE_FONT_8X14	// 3584 B
//#define  USE_FONT_10X16	// 8192 B
//#define  USE_FONT_12X16	// 8192 B
//#define  USE_FONT_12X20	// 10240 B
//#define  USE_FONT_16X26	// 13312 B
//#define  USE_FONT_22X36	// 27648 B
//#define  USE_FONT_24X40	// 30720 B
//#define  USE_FONT_32X53	// 54272 B

/* Specify platform-dependent integer types here */

#define __UG_FONT_DATA const PROGMEM
//#define READ_FONT_DATA(x) (x)
#define READ_FONT_DATA(x)  pgm_read_byte((uint16_t)&x)
typedef uint8_t      UG_U8;
typedef int8_t       UG_S8;
typedef uint16_t     UG_U16;
typedef int16_t      UG_S16;
typedef uint32_t     UG_U32;
typedef int32_t      UG_S32;

/* Example for dsPIC33
typedef unsigned char         UG_U8;
typedef signed char           UG_S8;
typedef unsigned int          UG_U16;
typedef signed int            UG_S16;
typedef unsigned long int     UG_U32;
typedef signed long int       UG_S32;
*/

/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */

/* Feature enablers */
#define USE_PRERENDER_EVENT
#define USE_POSTRENDER_EVENT

#endif
