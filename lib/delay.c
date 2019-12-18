#include <avr/io.h>
#include <avr/interrupt.h>

void delay_init()
{
    TCNT1 = 63974;

    TCCR1A = 0x00;
    // 1024 prescaller
    TCCR1B = (1 << CS10) | (1 << CS12);
    // timer overflow interrupt
    TIMSK0 = (1 << TOIE1);

    sei();
}

ISR (TIMER1_OVF_vect)
{
    PORTE ^= (1<<0);
    TCNT1 = 63974;
}
