#ifndef SPI_H
#define SPI_H

void SPI_master_init();
char SPI_master_transmit(char data);
char SPI_slave_read();
#endif
