/*
 * Ili9341.h
 *
 * Created: 3.3.2015 16:18:28
 *  Author: maticpi
 */ 

#ifndef ILI9341_H_
#define ILI9341_H_

#include <avr/io.h>
#include <stdio.h>
#include "config.h"
#include "ugui.h"

#ifndef F_CPU
	#define F_CPU	20000000UL
#endif
#include <util/delay.h>

#define LCD_USE_PRINTF

//#define ILI9341_SPCR	SPCR
//#define ILI9341_SPSR	SPSR
//#define ILI9341_SPDR	SPDR

#define ILI9341_SPCR	SPCR0
#define ILI9341_SPSR	SPSR0
#define ILI9341_SPDR	SPDR0

#define ILI9341_DDR_BACKLIGHT		DDRD
#define ILI9341_PORT_BACKLIGHT		PORTD
#define ILI9341_BIT_BACKLIGHT		6
#define ILI9341_BACKLIGHT_ON_VAL	0	//0 or 1 to turn on?		

#define ILI9341_DDR_MISO	DDRB
#define ILI9341_PORT_MISO	PORTB
#define ILI9341_PIN_MISO	PINB
#define ILI9341_BIT_MISO	4	
#define ILI9341_DDR_MOSI	DDRB
#define ILI9341_PORT_MOSI	PORTB
#define ILI9341_BIT_MOSI	3
#define ILI9341_DDR_SCK		DDRB
#define ILI9341_PORT_SCK	PORTB
#define ILI9341_BIT_SCK		5
#define ILI9341_DDR_DC		DDRB
#define ILI9341_PORT_DC		PORTB
#define ILI9341_BIT_DC		0
#define ILI9341_DDR_RST		DDRD
#define ILI9341_PORT_RST	PORTD
#define ILI9341_BIT_RST		7
#define ILI9341_DDR_CS		DDRB
#define ILI9341_PORT_CS		PORTB
#define ILI9341_BIT_CS		2

#if ILI9341_BACKLIGHT_ON_VAL ==	1	//0 or 1 to turn on?
	#define ILI9341_BACKLIGHT_ON()	ILI9341_PORT_BACKLIGHT |= (1<<ILI9341_BIT_BACKLIGHT)
	#define ILI9341_BACKLIGHT_OFF()	ILI9341_PORT_BACKLIGHT &= ~(1<<ILI9341_BIT_BACKLIGHT)
#else
	#define ILI9341_BACKLIGHT_ON()	ILI9341_PORT_BACKLIGHT &= ~(1<<ILI9341_BIT_BACKLIGHT)
	#define ILI9341_BACKLIGHT_OFF()	ILI9341_PORT_BACKLIGHT |= (1<<ILI9341_BIT_BACKLIGHT)
#endif
#define ILI9341_DC_CMD()	ILI9341_PORT_DC &= ~(1<<ILI9341_BIT_DC)
#define ILI9341_DC_DATA()	ILI9341_PORT_DC |= (1<<ILI9341_BIT_DC)

#define ILI9341_RST_ON()	ILI9341_PORT_RST &= ~(1<<ILI9341_BIT_RST)
#define ILI9341_RST_OFF()	ILI9341_PORT_RST |= (1<<ILI9341_BIT_RST)

#define ILI9341_CLK_LO()	ILI9341_PORT_SCK &= ~(1<<ILI9341_BIT_SCK)
#define ILI9341_CLK_HI()	ILI9341_PORT_SCK |= (1<<ILI9341_BIT_SCK)

#define ILI9341_CS_SELECT()		ILI9341_PORT_CS &= ~(1<<ILI9341_BIT_CS)
#define ILI9341_CS_DESELECT()	ILI9341_PORT_CS |= (1<<ILI9341_BIT_CS)


#define ILI9341_TFTWIDTH  240
#define ILI9341_TFTHEIGHT 320
//Level 1 commands
#define ILI9341_NOP     0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_RDDID   0x04
#define ILI9341_RDDST   0x09

#define ILI9341_SLPIN   0x10
#define ILI9341_SLPOUT  0x11
#define ILI9341_PTLON   0x12
#define ILI9341_NORON   0x13

#define ILI9341_RDMODE  0x0A
#define ILI9341_RDMADCTL  0x0B
#define ILI9341_RDPIXFMT  0x0C
#define ILI9341_RDIMGFMT  0x0A
#define ILI9341_RDSELFDIAG  0x0F

#define ILI9341_INVOFF  0x20
#define ILI9341_INVON   0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPOFF 0x28
#define ILI9341_DISPON  0x29

#define ILI9341_CASET   0x2A
#define ILI9341_PASET   0x2B
#define ILI9341_RAMWR   0x2C
#define ILI9341_RAMRD   0x2E

#define ILI9341_PTLAR   0x30	//Partial area
#define ILI9341_MADCTL  0x36	//Memory access control
#define ILI9341_PIXFMT  0x3A	//Pixel format

//Level 2 commands
#define ILI9341_FRMCTR1 0xB1	//Frame rate control in normal mode
#define ILI9341_FRMCTR2 0xB2	//Frame rate control in idle mode (8 colors) 
#define ILI9341_FRMCTR3 0xB3	//Frame rate control in partial mode
#define ILI9341_INVCTR  0xB4	//Display inversion control
#define ILI9341_DFUNCTR 0xB6	//Display function control

#define ILI9341_PWCTR1  0xC0	//Sets GVDD -default 0x21 = 4.5V
#define ILI9341_PWCTR2  0xC1
#define ILI9341_PWCTR3  0xC2
#define ILI9341_PWCTR4  0xC3
#define ILI9341_PWCTR5  0xC4
#define ILI9341_VMCTR1  0xC5
#define ILI9341_VMCTR2  0xC7

#define ILI9341_RDID1   0xDA	//Read ID1
#define ILI9341_RDID2   0xDB
#define ILI9341_RDID3   0xDC
#define ILI9341_RDID4   0xDD

#define ILI9341_GMCTRP1 0xE0	//Positive Gamma correction
#define ILI9341_GMCTRN1 0xE1	//Negative Gamma correction


// Color definitions
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F

extern uint16_t ILI9341_width;
extern uint16_t ILI9341_height;
extern uint8_t ILI9341_rotation;


void ILI9341_InitSPI();
uint8_t ILI9341_SPIwrite(uint8_t c);
#define ILI9341_SPIread()	ILI9341_SPIwrite(0x00)
//uint8_t ILI9341_SPIread();
void ILI9341_WriteCommand(uint8_t c);
void ILI9341_WriteData(uint8_t d);
uint8_t ILI9341_ReadCmd8(uint8_t c, uint8_t index);
void ILI9341_Init();
void ILI9341_Init_Basic();
void ILI9341_setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ILI9341_pushColor(uint16_t color);
void ILI9341_drawPixel(int16_t x, int16_t y, uint16_t color);
void ILI9341_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void ILI9341_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void ILI9341_fillScreen(uint16_t color);
void ILI9341_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
uint16_t ILI9341_color565(uint8_t r, uint8_t g, uint8_t b);
void ILI9341_setRotation(uint8_t m);
void ILI9341_invertDisplay(uint8_t i);

extern UG_GUI gui;

void UserPixelSetFunction (UG_S16 x , UG_S16 y ,UG_COLOR c);
UG_RESULT _HW_FillFrame_(UG_S16 x1 , UG_S16 y1 , UG_S16 x2 , UG_S16 y2 , UG_COLOR c );
void _HW_SetArea_(UG_S16 x1 , UG_S16 y1 , UG_S16 x2 , UG_S16 y2 );
void _HW_PushPixel(UG_COLOR color);

#ifdef LCD_USE_PRINTF
	int LCD_putchar(char c, FILE *stream);
	extern FILE LCD_stream;
#endif

void LCD_Init();

#endif /* ILI9341_H_ */