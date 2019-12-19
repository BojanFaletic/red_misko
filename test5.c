#include "delay.h"
#include "gpio.h"
#include "pirc_lcd/LCD_Ili9341.h"
#include "pirc_lcd/XPT2046.h"
#include "system.h"
#include "uart.h"
#include <avr/io.h>
#include <avr/sleep.h>
#include <stdbool.h>
#include <stdio.h>

#define MAX_OBJECTS 10

#define DEBUG_TOUCH_POS 0

// struct S_WINDOW window = *UG_WINDOW;

UG_WINDOW window;
UG_BUTTON button;
UG_OBJECT obj_list[MAX_OBJECTS];
UG_U8 btn_id = 1;
UG_U8 btn_home_id = 2;

void window_create(UG_WINDOW *wind, void *callback) {
  UG_WindowCreate(wind, obj_list, MAX_OBJECTS, callback);
}

void draw_title(UG_WINDOW *wind, char *title) {

  UG_WindowSetBackColor(wind, ILI9341_RED);
  UG_WindowSetTitleTextColor(wind, ILI9341_BLACK);
  UG_WindowSetTitleColor(wind, ILI9341_GREEN);
  UG_WindowSetTitleText(wind, title);
  UG_WindowSetTitleHeight(wind, 30);
  UG_WindowSetTitleInactiveColor(wind, ILI9341_GREEN);
  UG_WindowShow(wind);
}

void change_title(UG_WINDOW *wind, char *new_title) {
  UG_WindowSetTitleText(wind, new_title);
}

void change_button(UG_WINDOW *wind, char *btn_text) {
  UG_ButtonSetText(wind, btn_id, btn_text);
}

void window_1_callback(UG_MESSAGE *msg) {
  printf("Callback 1 function\n");
  if (button.state == TOUCH_STATE_PRESSED) {
    change_title(&window, "IP");
    change_button(&window, "ip");
  }
}

void create_home_button(UG_WINDOW *wind) {
  const int x_start = 0;
  const int x_end = 50;
  const int y_start = 170;
  const int y_end = 200;

  UG_ButtonCreate(wind, &button, btn_home_id, x_start, y_start, x_end, y_end);
  UG_ButtonSetForeColor(wind, btn_home_id, ILI9341_BLUE);
  UG_ButtonSetBackColor(wind, btn_home_id, ILI9341_YELLOW);
  UG_ButtonSetAlternateForeColor(&window, btn_home_id, ILI9341_MAGENTA);
  UG_ButtonSetAlternateBackColor(&window, btn_home_id, ILI9341_MAGENTA);
  UG_ButtonSetText(wind, btn_home_id, "H");
}

void button_show(UG_WINDOW *wind, uint8_t button) {
  UG_ButtonShow(wind, button);
}

void button_hide(UG_WINDOW *wind, uint8_t button) {
  UG_ButtonHide(wind, button);
}

int main() {
  LCD_Init();
  gpio_init();
  XPT2046_Init(240, 320);
  XPT2046_setRotation(ROT90);
  XPT2046_setCalibration(264, 122, 1972, 1775);

  uart_init();
  stdin = &uart_input;
  stdout = &uart_output;

  window_create(&window, window_1_callback);
  draw_title(&window, "HOME PAGE");
  create_home_button(&window);
  button_show(&window, btn_home_id);


  uint8_t led = 0;
  uint16_t x, y, x_prev, y_prev;

  while (1) {
    if (XPT2046_isTouching()) {
      // XPT2046_getPosition(&x,&y,MODE_DFR,32);
      XPT2046_getPosition(&x, &y, MODE_DFR, 32);
#if DEBUG_TOUCH_POS
      printf("Tuching, X = %d, Y = %d\n", x, y);
#endif
      UG_TouchUpdate(300 - x, y, TOUCH_STATE_PRESSED);

    } else {
      UG_TouchUpdate(x_prev, y_prev, TOUCH_STATE_RELEASED);
    }

    UG_Update();
    gpio_write(2, led);
    led ^= 0x1;
    _delay_ms(100);
  }
}
