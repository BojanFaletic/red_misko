#include "lib/delay.h"
#include "lib/gpio.h"
#include "lib/pirc_lcd/XPT2046.h"
#include "lib/uart.h"
#include "system/system.h"
#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>


#if 1

#include "lib/pirc_lcd/LCD_Ili9341.h"
void window_1_callback(UG_MESSAGE *msg) { printf("Callback function\n"); }
#define MAX_OBJECTS 10

void draw_home_scren() {
  UG_WINDOW window;
  UG_OBJECT obj_list[MAX_OBJECTS];
  UG_BUTTON button;
  UG_U8 btn_id = 1;
  struct cb *cb;

  printf("Hello\n");
  int status;
  status = UG_WindowCreate(&window, obj_list, MAX_OBJECTS, window_1_callback);
  if (status != UG_RESULT_OK)
    printf("UG_WindowCreate err!\n");

  UG_WindowSetBackColor(&window, ILI9341_BLACK);
  UG_WindowSetTitleTextColor(&window, ILI9341_BLACK);
  UG_WindowSetTitleColor(&window, ILI9341_GREEN);
  UG_WindowSetTitleText(&window, "IP config");
  UG_WindowSetTitleHeight(&window, 30);

  status = UG_ButtonCreate(&window, &button, btn_id, 10, 10, 110, 60);
  if (status != UG_RESULT_OK)
    printf("UG_ButtonCreate err!\n");

  status = UG_ButtonSetForeColor(&window, btn_id, ILI9341_BLACK);
  if (status != UG_RESULT_OK)
    printf("UG_ButtonSetForeColor err!\n");

  status = UG_ButtonSetBackColor(&window, btn_id, ILI9341_YELLOW);
  if (status != UG_RESULT_OK)
    printf("UG_ButtonSetBackColor err!\n");

  status = UG_ButtonShow(&window, btn_id);
  if (status != UG_RESULT_OK)
    printf("UG_ButtonShow err!\n");

  UG_WindowSetTitleInactiveColor(&window, ILI9341_GREEN);

  status = UG_WindowShow(&window);
  if (status != UG_RESULT_OK)
    printf("UG_WindowShow err!\n");

  UG_ButtonSetText(&window, btn_id, "IP");

  UG_Update();
  /*

      UG_ConsolePutString("Hello world\n");
      UG_DrawLine(0,0, 320,0, ILI9341_WHITE);
    */
}

#endif

int main() {
  LCD_Init();
  XPT2046_Init(240, 320);
  XPT2046_setCalibration(122, 264, 1775, 1972);

  uart_init();
  stdin = &uart_input;
  stdout = &uart_output;

  draw_home_scren();

  uint8_t update = 0;

  uint16_t x, y, x_prev, y_prev;
  while (1) {
    if (XPT2046_isTouching()) {
      // XPT2046_getPosition(&x,&y,MODE_DFR,32);
      XPT2046_getPosition(&x, &y, MODE_DFR, 32);
      printf("Tuching, X = %d, Y = %d\n", x, y);
      UG_TouchUpdate(x, y, TOUCH_STATE_PRESSED);
      
      x_prev = x;
      y_prev = y;
      update = 1;
    } else {
      UG_TouchUpdate(x_prev, y_prev, TOUCH_STATE_RELEASED);
      update = 1;
    }

    if (update == 1)
    {
        UG_Update();
        update = 0;
    }
    // printf("Done!\n");
    
  }
}
