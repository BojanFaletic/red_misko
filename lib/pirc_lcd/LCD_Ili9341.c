/*
 * CFile1.c
 *
 * Created: 3.3.2015 16:14:59
 *  Author: maticpi
 */ 

#include "LCD_Ili9341.h"
#include "ugui.h"

UG_GUI gui;
uint16_t ILI9341_width = ILI9341_TFTWIDTH;
uint16_t ILI9341_height = ILI9341_TFTHEIGHT;
uint8_t ILI9341_rotation;	//=0

#ifdef LCD_USE_PRINTF
	FILE LCD_stream = FDEV_SETUP_STREAM(LCD_putchar, NULL, _FDEV_SETUP_WRITE);
#endif

//////////////////////////////////////////////////////////
//             hardware interface functions
//////////////////////////////////////////////////////////
void ILI9341_InitSPI()
{
	ILI9341_DDR_MOSI |= (1<<ILI9341_BIT_MOSI);	//MOSI set as output
	ILI9341_DDR_MISO &= ~(1<<ILI9341_BIT_MISO);	//MISO set as input
	ILI9341_PORT_MISO &= ~(1<<ILI9341_BIT_MISO);	//pull-up on MISO off
	ILI9341_DDR_SCK |= (1<<ILI9341_BIT_SCK);	//SCK set as output
	ILI9341_DDR_CS |= (1<<ILI9341_BIT_CS);	//CS set as output
	ILI9341_DDR_DC |= (1<<ILI9341_BIT_DC);	//DC set as output
	ILI9341_DDR_RST |= (1<<ILI9341_BIT_RST);	//RST set as output
	
	ILI9341_SPCR = (0<<SPIE0) | (1<<SPE0) | (0<<DORD0) | (1<<MSTR0) | (0<< CPOL0) | (0<<CPHA0) | (0<<SPR00);
	ILI9341_SPSR = (1<<SPI2X0);
}

uint8_t ILI9341_SPIwrite(uint8_t c) 
{
	uint8_t result=0;

	ILI9341_SPDR = c;
    while(!(ILI9341_SPSR & (1<<SPIF0)));
	result = ILI9341_SPDR;
	return result;
}

void ILI9341_WriteCommand(uint8_t c)
{
	ILI9341_DC_CMD();			//DC=0
	ILI9341_CS_SELECT();		//CS=0
	ILI9341_SPIwrite(c);
	ILI9341_CS_DESELECT();	//CS=1
}

void ILI9341_WriteData(uint8_t d)
{
	ILI9341_DC_DATA();		//DC=1
	ILI9341_CS_SELECT();		//CS=0
	ILI9341_SPIwrite(d);
	ILI9341_CS_DESELECT();	//CS=1
} 

uint8_t ILI9341_ReadCmd8(uint8_t c, uint8_t index)
{
	uint8_t result;
	ILI9341_DC_CMD();			//DC=0
	ILI9341_CS_SELECT();		//CS=0
	ILI9341_SPIwrite(0xD9);
	ILI9341_DC_DATA();		//DC=1
	ILI9341_SPIwrite(0x10+index);
	ILI9341_CS_DESELECT();	//CS=1

	ILI9341_DC_CMD();			//DC=0
	ILI9341_CLK_LO();			//CLK=0
	ILI9341_CS_SELECT();		//CS=0
	ILI9341_SPIwrite(c);
	
	ILI9341_DC_DATA();		//DC=1
	result = ILI9341_SPIread();
	ILI9341_CS_DESELECT();	//CS=1
	return result;
}

//////////////////////////////////////////////////////////
//             basic low level functions
//////////////////////////////////////////////////////////
void ILI9341_Init()
{
	ILI9341_RST_ON();
	ILI9341_InitSPI();
	ILI9341_DDR_BACKLIGHT |= (1<<ILI9341_BIT_BACKLIGHT);
	ILI9341_RST_OFF();
	_delay_ms(5);
	ILI9341_RST_ON();
	_delay_ms(20);
	ILI9341_RST_OFF();
	_delay_ms(150);

	ILI9341_setRotation(3);
	ILI9341_WriteCommand(ILI9341_PIXFMT);
	ILI9341_WriteData(0x55); // (def. = 0x66)
	ILI9341_WriteCommand(ILI9341_SLPOUT);    //Exit Sleep
	_delay_ms(120);
	ILI9341_WriteCommand(ILI9341_DISPON);    //Display on
	ILI9341_BACKLIGHT_ON();
}

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

void ILI9341_setRotation(uint8_t m)
{
	ILI9341_WriteCommand(ILI9341_MADCTL);
	ILI9341_rotation = m % 4; // can't be higher than 3
	switch (ILI9341_rotation)
	{
		case 0:
			ILI9341_WriteData(MADCTL_MX | MADCTL_BGR);
			ILI9341_width  = ILI9341_TFTWIDTH;
			ILI9341_height = ILI9341_TFTHEIGHT;
			break;
		case 1:
			ILI9341_WriteData(MADCTL_MV | MADCTL_BGR);
			ILI9341_width  = ILI9341_TFTHEIGHT;
			ILI9341_height = ILI9341_TFTWIDTH;
			break;
		case 2:
			ILI9341_WriteData(MADCTL_MY | MADCTL_BGR);
			ILI9341_width  = ILI9341_TFTWIDTH;
			ILI9341_height = ILI9341_TFTHEIGHT;
			break;
		case 3:
			ILI9341_WriteData(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
			ILI9341_width  = ILI9341_TFTHEIGHT;
			ILI9341_height = ILI9341_TFTWIDTH;
			break;
	}
}

void ILI9341_setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	ILI9341_WriteCommand(ILI9341_CASET); // Column addr set
	ILI9341_WriteData(x0 >> 8);
	ILI9341_WriteData(x0 & 0xFF);     // XSTART 
	ILI9341_WriteData(x1 >> 8);
	ILI9341_WriteData(x1 & 0xFF);     // XEND

	ILI9341_WriteCommand(ILI9341_PASET); // Row addr set
	ILI9341_WriteData(y0>>8);
	ILI9341_WriteData(y0);     // YSTART
	ILI9341_WriteData(y1>>8);
	ILI9341_WriteData(y1);     // YEND

	ILI9341_WriteCommand(ILI9341_RAMWR); // write to RAM
}

void ILI9341_pushColor(uint16_t color)
{
	ILI9341_DC_DATA();
	ILI9341_CS_SELECT();
	ILI9341_SPIwrite(color >> 8);
	ILI9341_SPIwrite(color);
	ILI9341_CS_DESELECT();
}

//////////////////////////////////////////////////////////
//             basic drawing functions
//////////////////////////////////////////////////////////
void ILI9341_drawPixel(int16_t x, int16_t y, uint16_t color)
{
	if((x < 0) ||(x >= ILI9341_width) || (y < 0) || (y >= ILI9341_height)) return;
	ILI9341_setAddrWindow(x,y,x+1,y+1);

	ILI9341_DC_DATA();
	ILI9341_CS_SELECT();
	ILI9341_SPIwrite(color >> 8);
	ILI9341_SPIwrite(color);
	ILI9341_CS_DESELECT();
}

void ILI9341_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	uint8_t hi = color >> 8, lo = color;
	
	if((x >= ILI9341_width) || (y >= ILI9341_height)) return;

	if((y+h-1) >= ILI9341_height) 
		h = ILI9341_height-y;

	ILI9341_setAddrWindow(x, y, x, y+h-1);
	ILI9341_DC_DATA();
	ILI9341_CS_SELECT();
	while (h--)
	{
		ILI9341_SPIwrite(hi);
		ILI9341_SPIwrite(lo);
	}
	ILI9341_CS_DESELECT();
}

void ILI9341_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) 
{
	uint8_t hi = color >> 8, lo = color;
	
	if((x >= ILI9341_width) || (y >= ILI9341_height)) return;
	if((x+w-1) >= ILI9341_width)  w = ILI9341_width-x;

	ILI9341_setAddrWindow(x, y, x+w-1, y);

	ILI9341_DC_DATA();
	ILI9341_CS_SELECT();
	while (w--)
	{
		ILI9341_SPIwrite(hi);
		ILI9341_SPIwrite(lo);
	}
	ILI9341_CS_DESELECT();
}

void ILI9341_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	uint8_t hi = color >> 8, lo = color;
  
	if((x >= ILI9341_width) || (y >= ILI9341_height)) return;
	if((x + w - 1) >= ILI9341_width)  w = ILI9341_width  - x;
	if((y + h - 1) >= ILI9341_height) h = ILI9341_height - y;

	ILI9341_setAddrWindow(x, y, x+w-1, y+h-1);

	ILI9341_DC_DATA();
	ILI9341_CS_SELECT();

	for(y=h; y>0; y--)
	{
		for(x=w; x>0; x--)
		{
			ILI9341_SPIwrite(hi);
			ILI9341_SPIwrite(lo);
		}
	}
	ILI9341_CS_DESELECT();
}

void ILI9341_fillScreen(uint16_t color)
{
	ILI9341_fillRect(0, 0,  ILI9341_width, ILI9341_height, color);
}

void ILI9341_invertDisplay(uint8_t i)
{
	ILI9341_WriteCommand(i ? ILI9341_INVON : ILI9341_INVOFF);
}

///////////////////////////////////////////////////////////////////
//					Interface for uGUI
///////////////////////////////////////////////////////////////////

void UserPixelSetFunction (UG_S16 x , UG_S16 y ,UG_COLOR c)
{
	ILI9341_drawPixel(x,y,c);
}

/* Hardware accelerator for UG_FillFrame */
UG_RESULT _HW_FillFrame_(UG_S16 x1 , UG_S16 y1 , UG_S16 x2 , UG_S16 y2 , UG_COLOR c )
{
	ILI9341_fillRect(x1,y1,x2-x1,y2-y1,c);
	return UG_RESULT_OK;
}

void _HW_SetArea_(UG_S16 x1 , UG_S16 y1 , UG_S16 x2 , UG_S16 y2 )
{
	ILI9341_setAddrWindow(x1,y1,x2,y2);
}

void _HW_PushPixel(UG_COLOR color)
{
	ILI9341_pushColor(color);
}

///////////////////// printf interface //////////////////
#ifdef LCD_USE_PRINTF
	int LCD_putchar(char c, FILE *stream)
	{
		char txt[2];
		txt[0]=c;
		txt[1]=0;
		UG_ConsolePutString(txt);
		return 0;
	}
#endif

//////////////////// HIGH LEVEL INIT ////////////////////
void LCD_Init()
{
	ILI9341_Init();
	ILI9341_DDR_BACKLIGHT |= (1<<ILI9341_BIT_BACKLIGHT);
	ILI9341_BACKLIGHT_ON();
	UG_Init(&gui , UserPixelSetFunction , ILI9341_width , ILI9341_height) ;
	UG_DriverRegister(DRIVER_FILL_FRAME, ( void *) _HW_FillFrame_ );
	UG_DriverRegister(DRIVER_SET_AREA, ( void *) _HW_SetArea_ );
	UG_DriverRegister(DRIVER_PUSH_PIXEL, ( void *) _HW_PushPixel );
	UG_DriverEnable(DRIVER_FILL_FRAME);
	UG_DriverEnable(DRIVER_SET_AREA);
	UG_DriverEnable(DRIVER_PUSH_PIXEL);

	ILI9341_fillRect(0,0,320,240,ILI9341_BLACK);
	UG_FontSelect ( &RFONT_8X12 ) ;
	UG_SetForecolor ( ILI9341_WHITE ) ;
	UG_ConsoleSetBackcolor ( ILI9341_BLACK ) ;
	UG_ConsoleSetForecolor ( ILI9341_WHITE ) ;
	UG_ConsoleSetArea(1,1,318,238);
#ifdef LCD_USE_PRINTF
	stdout=&LCD_stream;
#endif
}
