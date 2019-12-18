#include <stdio.h>
#include <stdbool.h>
#include "lib/uart.h"
#include "lib/io_interrupt.h"
#include "lib/gpio.h"

int main(){
    uart_init();
    //gpio_init();
    //io_interrupt_enable(0);
    //io_interrupt_enable(1);
    
    while(1)
    {
        printf("Hello world!!\n");
    }
    return 0;
    
};
