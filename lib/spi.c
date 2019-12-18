#include <avr/io.h>

#define SPI0_MOSI PINB3
#define SPI0_MISO PINB4
#define SPI0_SCK PINB5
#define SPI0_SS  PINB2

void SPI_master_init()
{
    // Set outputs
    DDRB |= _BV(SPI0_MOSI) | _BV(SPI0_SCK) | _BV(SPI0_SS); 
    DDRB &= ~_BV(SPI0_MISO);
    PORTB &= ~_BV(SPI0_MISO);
    // Enable SPI
    SPCR0 = (0<<SPIE0) | (1<<SPE0) | (0<<DORD0) | (1<<MSTR0) | (0<< CPOL0) | (0<<CPHA0) | (0<<SPR00);
    // Clock rate 2x
    SPSR0 = _BV(SPI2X0);
}


char SPI_master_transmit(char data)
{
    uint8_t result = 0;
    SPDR0 = data;
    // wait until transmittion is complite
    while(!(SPSR0 & _BV(SPIF0)));
    result = SPDR0;
    return result;
}

char SPI_slave_read()
{
    // wait until data is ready
    while(!(SPSR0 & _BV(SPIF0)));
    return SPDR0;
}
