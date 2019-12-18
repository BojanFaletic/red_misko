#include <avr/io.h>

void gpio_init()
{
    DDRE = 0x0f;
    PIND |= _BV(5);
}

void gpio_write(char pin, char value)
{
    if (value == 1){
        PORTE |= (1 << pin); 
    }
    else if (value == 0){
        PORTE &= ~(1 << pin);
    }
}

int gpio_read(char pin)
{
    return PIND & _BV(pin);
}
