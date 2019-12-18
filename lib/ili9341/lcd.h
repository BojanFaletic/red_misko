#ifndef _LCD_H_
#define _LCD_H_

#define PORTRAIT 0
#define LANDSCAPE 1

void lcd_ddr_init();

void lcd_cs_low();

void lcd_cs_high();

void lcd_dc_low();

void lcd_dc_high();

void backlight_off();

void lcd_backlight_on();

void lcd_rst_on();

void lcd_rst_off();

void lcd_send_cmd(char data);

void lcd_send_data(char data);

void lcd_send_data16(uint16_t data);

void lcd_setOrientation(uint8_t orientation);

void lcd_set_x(uint16_t x0, uint16_t x1);

void lcd_set_y(uint16_t y0, uint16_t y1);

void lcd_setXY(uint16_t x0, uint16_t y0);

void lcd_pixel(uint16_t x0, uint16_t y0, uint16_t color);

void lcd_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
    uint16_t color);

void lcd_hline(uint16_t x0, uint16_t y0, uint16_t length,
    uint16_t color);

void lcd_vline(uint16_t x0, uint16_t y0, uint16_t length,
    uint16_t color);

void lcd_rectange(uint16_t x0, uint16_t y0, uint16_t length,
   uint16_t width, uint16_t color);

void lcd_fill_rectange(uint16_t x0, uint16_t y0, uint16_t length,
    uint16_t width, uint16_t color);

void lcd_bg(uint16_t color);

void lcd_circle(uint16_t x0, uint16_t y0, uint16_t r,
    uint16_t color);

void lcd_init();


#endif