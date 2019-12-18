#include <stdio.h>
#include <avr/io.h>
#include "system/system.h"
#include "lib/uart.h"
#include "lib/gpio.h"
#include "lib/delay.h"
#include "lib/pirc_lcd/LCD_Ili9341.h"
#include <stdbool.h>

volatile int a = 0;
Mutex *mutex = NULL;



void task08(void *paramters)
{
    while(1){
        osMutexLock(mutex);
        printf("Hello \n");
        osMutexUnlock(mutex);
        osWait(100);
    }
}

void task09(void *paramters)
{
    while(1)
    {
        osMutexLock(mutex);
        printf("world \n");
        osMutexUnlock(mutex);
        osWait(100);
    }
}





typedef struct obj_draw_str
{
    uint16_t prev_x, prev_y;
    uint16_t prev_h, prev_w;
    uint16_t prev_color;
} obj_draw;


typedef struct mem_screen_type
{
    uint8_t active_screen[240/2][320/2];
} mem_screen;


void draw_rect(obj_draw *obj, int x, int y, int w, int h,int color)
{
    int dx = x-obj->prev_x;

    ILI9341_fillRect(x+w,0,1,w,ILI9341_RED);
    ILI9341_fillRect(x-1,0,1,w,ILI9341_BLUE);

    obj->prev_x = x;
    obj->prev_x = y;
    obj->prev_h = h;
    obj->prev_w = w;
    
   
}



int main(int argc, char* argv[])
{
    mem_screen screen_buf;

    gpio_init();
    //LCD_Init();
#if 0
    //ILI9341_drawFastHLine(20,80,10,ILI9341_RED);
    int i = 0;
    ILI9341_fillRect(i,0,20,20,ILI9341_RED);

    volatile obj_draw screen;

    memset(screen_buf.active_screen, 11, 240/2*320/2);

    for (int x=0; x<240;x++){
        for (int y=0; y<320; y++)
        {
            UserPixelSetFunction(x,y, screen_buf.active_screen[x/2][y/2]);
        }
    }

    while(1)
    {
        draw_rect(&screen, i,0,20,20,ILI9341_RED);
        _delay_ms(5);
        
        i++;
        if (i > 240)
        {
            screen.prev_x = 0;
            ILI9341_fillRect(i-1,0,21,20,ILI9341_BLUE);
            ILI9341_fillRect(0,0,20,20,ILI9341_RED);
            i = 0;
        }
    }
#endif
    while(1);

    uart_init();
    stdin = &uart_input;
    stdout = &uart_output;

    // initialize operating system
    osInit();

    // create lockable object
    mutex = osMutexCreate();

    osCreateTask(task08, NULL, 64, 5);
    osCreateTask(task09, NULL, 64, 5);

    osRun();
}
