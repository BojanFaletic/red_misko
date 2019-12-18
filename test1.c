#include <avr/interrupt.h>

#include "lib/gpio.h"
#include "lib/io_interrupt.h"

#define DELAY 16000

int main()
{
    gpio_init();
    io_interrupt_enable(0);
    io_interrupt_enable(1);
    io_pullup_enable(0);
    io_pullup_enable(1);

    while(1)
    {
        gpio_write(1,1);
        for (volatile unsigned long i = 0; i < DELAY; i++);
        gpio_write(1,0);
        for (volatile unsigned long i = 0; i < DELAY; i++);
    }
}


ISR(INT0_vect)
{
    gpio_write(2,1);
}

ISR(INT1_vect)
{
    gpio_write(2,0);
}

