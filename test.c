#include "lib/gpio.h"


#define DELAY 16000

int main()
{
    gpio_init();

    while(1)
    {
        gpio_write(1,1);
        for (volatile unsigned long i = 0; i < DELAY; i++);
        gpio_write(1,0);
        for (volatile unsigned long i = 0; i < DELAY; i++);
    }
}
