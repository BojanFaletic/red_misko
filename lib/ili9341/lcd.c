#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

#include "lcd.h"
#include "ili9341cmd.h"
#include "font.c"
#include "../spi.h"
#include "../gpio.h"
#include "colors.h"

#define SPI_DIRECTION_PORT DDRB
#define SPI_PORT PORTB
#define SPI_CS PINB2
#define SPI_DC PINB0

#define CONTROL_DIRECTION_PORT DDRD
#define CONTROL_PORT PORTD
#define BACKLIGHT_PIN PIND6
#define RESET PIND7

uint16_t lcd_max_x, lcd_max_y;
uint8_t lcd_orientation;
uint16_t lcd_bg_color = black;

void lcd_ddr_init()
{
    SPI_DIRECTION_PORT |= _BV(SPI_CS) | _BV(SPI_DC);
    CONTROL_DIRECTION_PORT |= _BV(BACKLIGHT_PIN) | _BV(RESET); 
}


void lcd_cs_low()
{
	SPI_PORT &=~ _BV(SPI_CS);
}

void lcd_cs_high()
{
	SPI_PORT |= _BV(SPI_CS);
}

void lcd_dc_low()
{
	SPI_PORT &= ~_BV(SPI_DC);
}

void lcd_dc_high()
{
	SPI_PORT |= _BV(SPI_DC);
}

void backlight_off()
{
	CONTROL_PORT |= _BV(BACKLIGHT_PIN);
}

void lcd_backlight_on()
{
	CONTROL_PORT &= ~_BV(BACKLIGHT_PIN);
}

void lcd_rst_on()
{
    CONTROL_PORT |= _BV(RESET);   
}

void lcd_rst_off()
{
    CONTROL_PORT &= ~_BV(RESET);   
}


void lcd_send_cmd(char data)
{
    lcd_dc_low();
    lcd_cs_low();
    SPI_master_transmit(data);
    lcd_cs_high();
}

void lcd_send_data(char data)
{
    lcd_dc_high();
    lcd_cs_low();
    SPI_master_transmit(data);
    lcd_cs_high();
}

void lcd_send_data16(uint16_t data)
{
    uint8_t data_high = (uint8_t)(data >> 8);
    uint8_t data_low = (uint8_t)(data & 0xff);
    lcd_dc_high();
    lcd_cs_low();
    SPI_master_transmit(data_high);
    SPI_master_transmit(data_low);
    lcd_cs_high();
}

void lcd_setOrientation(uint8_t orientation)
{
    if(orientation == PORTRAIT)
    {
        lcd_max_x = 240;
        lcd_max_y = 320;
    }
    else if (orientation == LANDSCAPE)
    {
        lcd_max_x = 320;
        lcd_max_y = 240;
    }
    lcd_orientation = orientation;
}

void lcd_set_x(uint16_t x0, uint16_t x1)
{
    lcd_send_cmd(ILI9341_CMD_COLUMN_ADDRESS_SET);
    lcd_send_data16(x0);
    lcd_send_data16(x1);
}

void lcd_set_y(uint16_t y0, uint16_t y1)
{
    lcd_send_cmd(ILI9341_CMD_PAGE_ADDRESS_SET);
    lcd_send_data16(y0);
    lcd_send_data16(y1);
}

void lcd_setXY(uint16_t x0, uint16_t y0)
{
    lcd_set_x(x0, x0);
    lcd_set_y(y0, y0);
    lcd_send_cmd(ILI9341_CMD_MEMORY_WRITE);
}


void lcd_pixel(uint16_t x0, uint16_t y0, uint16_t color)
{
    if (lcd_orientation == PORTRAIT)
    {
        lcd_setXY(x0, y0);
        lcd_send_data16(color);
    }
    else if (lcd_orientation == LANDSCAPE)
    {
        lcd_setXY(y0, lcd_max_x-x0);
        lcd_send_data16(color);
    }
}


void lcd_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
    uint16_t color)
{
    int16_t x = x1-x0;
    int16_t y = y1-y0;

    int16_t dx = abs(x), sx = x0<x1 ? 1 : -1;
    int16_t dy = -abs(y), sy = y0<y1 ? 1 : -1;

    int err = dx+dy, e2;
    while(1)
    {
        lcd_pixel(x0, y0, color);
        e2 = 2*err;
        if (e2 >= dy)
        {
            if (x0 == x1)
            {
                break;
            }
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            if (y0 == y1)
            {
                break;
            }
            err += dx;
            y0 += sy;
        }
    }
}

void lcd_hline(uint16_t x0, uint16_t y0, uint16_t length,
    uint16_t color)
{
    if (lcd_orientation == PORTRAIT)
    {
        lcd_set_x(x0, x0+length);
        lcd_set_y(y0, y0);
        lcd_send_cmd(ILI9341_CMD_MEMORY_WRITE);
        for (int i=0; i<length; i++)
        {
            lcd_send_data16(color);
        }
    }
    else if (lcd_orientation == LANDSCAPE)
    {
        lcd_set_x(y0, y0);
        lcd_set_y(lcd_max_x-x0-length, lcd_max_x-x0);
        lcd_send_cmd(ILI9341_CMD_MEMORY_WRITE);
        for (int i=0; i<length; i++)
        {
            lcd_send_data16(color);
        }
    }
}

void lcd_vline(uint16_t x0, uint16_t y0, uint16_t length,
    uint16_t color)
{
    if (lcd_orientation == PORTRAIT)
    {
        lcd_set_x(x0, x0);
        lcd_set_y(y0, y0+length);
        lcd_send_cmd(ILI9341_CMD_MEMORY_WRITE);
        for (int i=0; i<length; i++)
        {
            lcd_send_data16(color);
        }
    }
    else if (lcd_orientation == LANDSCAPE)
    {
        lcd_set_x(y0, y0 + length);
        lcd_set_y(lcd_max_x-x0, lcd_max_x-x0);
        lcd_send_cmd(ILI9341_CMD_MEMORY_WRITE);
        for (int i=0; i<length; i++)
        {
            lcd_send_data16(color);
        }
    }
}

void lcd_rectange(uint16_t x0, uint16_t y0, uint16_t length,
   uint16_t width, uint16_t color)
{
    lcd_hline(x0, y0, length, color);
    lcd_hline(x0, y0+width, length, color);
    lcd_vline(x0, y0, width, color);
    lcd_vline(x0+length, y0, width, color);
}

void lcd_fill_rectange(uint16_t x0, uint16_t y0, uint16_t length,
    uint16_t width, uint16_t color)
{
    for (int i=0; i<width; i++)
    {
        lcd_hline(x0, y0+i, length, color);
    }
}

void lcd_bg(uint16_t color)
{
    lcd_fill_rectange(0,0,lcd_max_x, lcd_max_y, color);
}


void lcd_circle(uint16_t x0, uint16_t y0, uint16_t r,
    uint16_t color)
{
    int16_t x = -r, y=0, err = 2-2*r, e2;
    do
    {
       lcd_pixel(x0-x, y0+y, color);
       lcd_pixel(x0+x, y0+y, color);
       lcd_pixel(x0+x, y0-y, color);
       lcd_pixel(x0-x, y0-y, color);
       e2 = err;
       if (e2 <= y)
       {
           err += ++y*2+1;
           if (-x == y && e2 <= x)
           {
               e2 = 0;
           }
       }
       if (e2 > x)
       {
           err += ++x*2+1;
       }
    } while(x <= 0);
}

void lcd_init()
{
    lcd_ddr_init();
    SPI_master_init();
    lcd_cs_high();
    lcd_dc_high();

    lcd_rst_on();
    _delay_ms(10);
    lcd_rst_off();

    lcd_setOrientation(PORTRAIT);

    lcd_send_cmd(ILI9341_CMD_POWER_ON_SEQ_CONTROL);
    lcd_send_data(ILI9341_CMD_IDLE_MODE_ON);
    lcd_send_data(ILI9341_CMD_MEMORY_WRITE);
    lcd_send_data(ILI9341_CMD_NOP);
    lcd_send_data(ILI9341_CMD_TEARING_EFFECT_LINE_OFF);
    lcd_send_data(0x02);

    lcd_send_cmd(ILI9341_CMD_POWER_CONTROL_B);
    lcd_send_data(ILI9341_CMD_NOP);
    lcd_send_data(ILI9341_CMD_POWER_CONTROL_2);
    lcd_send_data(ILI9341_CMD_PARTIAL_AREA);

    lcd_send_cmd(ILI9341_CMD_DRIVER_TIMING_CONTROL_A);
    lcd_send_data(0x85);
    lcd_send_data(ILI9341_CMD_NOP);
    lcd_send_data(0x78);

    lcd_send_cmd(ILI9341_CMD_DRIVER_TIMING_CONTROL_B);
    lcd_send_data(ILI9341_CMD_NOP);
    lcd_send_data(ILI9341_CMD_NOP);

    lcd_send_cmd(0xED);
    lcd_send_data(0x64);
    lcd_send_data(0x03);
    lcd_send_data(ILI9341_CMD_PARTIAL_MODE_ON);
    lcd_send_data(0x81);

    lcd_send_cmd(ILI9341_CMD_PUMP_RATIO_CONTROL);
    lcd_send_data(ILI9341_CMD_DISP_INVERSION_OFF);

    lcd_send_cmd(ILI9341_CMD_POWER_CONTROL_1);
    lcd_send_data(0x23);

    lcd_send_cmd(ILI9341_CMD_POWER_CONTROL_2);
    lcd_send_data(ILI9341_CMD_ENTER_SLEEP_MODE);

    lcd_send_cmd(ILI9341_CMD_VCOM_CONTROL_1);
    lcd_send_data(ILI9341_CMD_READ_MEMORY_CONTINUE);
    lcd_send_data(ILI9341_CMD_DISPLAY_OFF);

    lcd_send_cmd(ILI9341_CMD_VCOM_CONTROL_2);
    lcd_send_data(0x86);

    lcd_send_cmd(ILI9341_CMD_MEMORY_ACCESS_CONTROL);
    lcd_send_data(0x48);

    lcd_send_cmd(ILI9341_CMD_COLMOD_PIXEL_FORMAT_SET);
    lcd_send_data(ILI9341_CMD_WRITE_CONTENT_ADAPT_BRIGHTNESS);

    lcd_send_cmd(ILI9341_CMD_FRAME_RATE_CONTROL_NORMAL);
    lcd_send_data(ILI9341_CMD_NOP);
    lcd_send_data(0x18);

    lcd_send_cmd(ILI9341_CMD_DISPLAY_FUNCTION_CONTROL);
    lcd_send_data(0x08);
    lcd_send_data(0x82);
    lcd_send_data(0x27);

    lcd_send_cmd(ILI9341_CMD_ENABLE_3_GAMMA_CONTROL);
    lcd_send_data(ILI9341_CMD_NOP);

    // Select Gamma curve 
    lcd_send_cmd(0x26);
    lcd_send_data(ILI9341_CMD_SOFTWARE_RESET);

    lcd_send_cmd(ILI9341_CMD_POSITIVE_GAMMA_CORRECTION);
    lcd_send_data(0x0f);
    lcd_send_data(0x31);
    lcd_send_data(ILI9341_CMD_PAGE_ADDRESS_SET);
    lcd_send_data(ILI9341_CMD_READ_DISP_PIXEL_FORMAT);
    lcd_send_data(ILI9341_CMD_READ_DISP_SIGNAL_MODE);
    lcd_send_data(0x08);
    lcd_send_data(0x4e);
    lcd_send_data(0xf1);
    lcd_send_data(ILI9341_CMD_VERT_SCROLL_START_ADDRESS);
    lcd_send_data(0x07);
    lcd_send_data(ILI9341_CMD_ENTER_SLEEP_MODE);
    lcd_send_data(0x03);
    lcd_send_data(ILI9341_CMD_READ_DISP_SIGNAL_MODE);
    lcd_send_data(ILI9341_CMD_READ_DISP_STATUS);
    lcd_send_data(ILI9341_CMD_NOP);

    lcd_send_cmd(ILI9341_CMD_NEGATIVE_GAMMA_CORRECTION);
    lcd_send_data(ILI9341_CMD_NOP);
    lcd_send_data(ILI9341_CMD_READ_DISP_SIGNAL_MODE);
    lcd_send_data(0x14);
    lcd_send_data(0x03);
    lcd_send_data(ILI9341_CMD_SLEEP_OUT);
    lcd_send_data(0x07);
    lcd_send_data(0x31);
    lcd_send_data(ILI9341_CMD_POWER_CONTROL_2);
    lcd_send_data(0x48);
    lcd_send_data(0x08);
    lcd_send_data(0x0f);
    lcd_send_data(ILI9341_CMD_READ_DISP_PIXEL_FORMAT);
    lcd_send_data(0x31);
    lcd_send_data(ILI9341_CMD_MEMORY_ACCESS_CONTROL);
    lcd_send_data(ILI9341_CMD_READ_DISP_SELF_DIAGNOSTIC);

    lcd_send_cmd(ILI9341_CMD_SLEEP_OUT);
    _delay_ms(120);

    lcd_send_cmd(ILI9341_CMD_DISPLAY_ON);
    lcd_send_cmd(ILI9341_CMD_MEMORY_WRITE);
    lcd_bg(lcd_bg_color);
    //lcd_set

}
