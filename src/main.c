
#include <stdio.h>
#include <avr/io.h>

#include "system.h"
#include "uart.h"
#include "gpio.h"
#include "delay.h"
#include "pirc_lcd/LCD_Ili9341.h"
#include <stdbool.h>
#include "gui.h"



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


void platform_init()
{
    gpio_init();
    // LCD_Init();

    uart_init();
    stdin = &uart_input;
    stdout = &uart_output;

    printf("Init done \n");
}


void os()
{
    osRun();
}


int main()
{
    platform_init();

    osInit();
    /*
    uart_init();
    stdin = &uart_input;
    stdout = &uart_output;
    */

    // create lockable object
    mutex = osMutexCreate();



    osCreateTask(task08, NULL, 128, 1);
    osCreateTask(task09, NULL, 64, 5);

    os();

    printf("Error this shuld never rech \n");


    return 0;
    
}
