#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>



void io_interrupt_enable(int ext_int_number)
{
    // Negative edge
    EICRA |= ext_int_number ? _BV(ISC11) : _BV(ISC01); 
    EIMSK |= ext_int_number ? _BV(INT1) : _BV(INT0);
    sei();
}


void io_pullup_enable(int io_number)
{
    PORTD |= io_number ? _BV(3) : _BV(2);
}

/*
    Copy function in main file to use coresponding io interrupt

ISR(PCINT0_vect)
{
}

ISR(PCINT1_vect)
{
}

*/
