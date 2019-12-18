/*
 * Copyright (c) 2015-2016  Spiros Papadimitriou
 *
 * This file is part of github.com/spapadim/XPT2046 and is released
 * under the MIT License: https://opensource.org/licenses/MIT
 *
 * This software is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied.
 */

#ifndef _XPT2046_h_
#define _XPT2046_h_

#include <avr/io.h>
#include <stdint.h>

#define SHARE_SPI

#define XPT2046_SPCR	SPCR0
#define XPT2046_SPSR	SPSR0
#define XPT2046_SPDR	SPDR0

//Tspi_clk >= 400ns => <2.5MHz
//F_CPU=16MHz/8 = 2MHz
#define 	XPT2046_SPCR_value ((0<<SPIE0) | (1<<SPE0) | (0<<DORD0) | (1<<MSTR0) | (0<< CPOL0) | (0<<CPHA0) | (1<<SPR00))
#define 	XPT2046_SPSR_value (1<<SPI2X0)

#define XPT2046_DDR_MISO	DDRB
#define XPT2046_PORT_MISO	PORTB
#define XPT2046_PIN_MISO	PINB
#define XPT2046_BIT_MISO	4
#define XPT2046_DDR_MOSI	DDRB
#define XPT2046_PORT_MOSI	PORTB
#define XPT2046_BIT_MOSI	3
#define XPT2046_DDR_SCK		DDRB
#define XPT2046_PORT_SCK	PORTB
#define XPT2046_BIT_SCK		5

#define XPT2046_DDR_CS		DDRD
#define XPT2046_PORT_CS		PORTD
#define XPT2046_BIT_CS		3
#define XPT2046_DDR_IRQ		DDRD
#define XPT2046_PORT_IRQ	PORTD
#define XPT2046_PIN_IRQ		PIND
#define XPT2046_BIT_IRQ		2

#define XPT2046_CS_SELECT()		XPT2046_PORT_CS &= ~(1<<XPT2046_BIT_CS)
#define XPT2046_CS_DESELECT()	XPT2046_PORT_CS |= (1<<XPT2046_BIT_CS)

#define IS_XPT2046_IRQ_LOW()	!(XPT2046_PIN_IRQ & (1<<XPT2046_BIT_IRQ))
#define IS_XPT2046_IRQ_HIGH()	(XPT2046_PIN_IRQ & (1<<XPT2046_BIT_IRQ))

// On my display driver ICs i,j mapped to (width-y),x
//  Flipping can be handled by order of calibration points, but not swapping
#if !defined(SWAP_AXES)
#  define SWAP_AXES 1
#endif

extern uint16_t CAL_MARGIN;

uint8_t XPT2046_SPIwrite(uint8_t c);
uint16_t XPT2046_SPIwrite16(uint16_t c);
 
enum rotation_t { ROT0, ROT90, ROT180, ROT270 };
enum adc_ref_t  { MODE_SER, MODE_DFR };

void XPT2046_Init(uint16_t width, uint16_t height);  // width and height with no rotation!
void XPT2046_setRotation(enum rotation_t rot);

// Calibration needs to be done with no rotation, on both display and touch drivers
void XPT2046_getCalibrationPoints(uint16_t *x1, uint16_t *y1, uint16_t *x2, uint16_t *y2);
void XPT2046_setCalibration (uint16_t vi1, uint16_t vj1, uint16_t vi2, uint16_t vj2);

uint8_t XPT2046_isTouching();

void XPT2046_getRaw(uint16_t *vi, uint16_t *vj, enum adc_ref_t mode, uint8_t max_samples);
void XPT2046_getPosition(uint16_t *x, uint16_t *y, enum adc_ref_t mode, uint8_t max_samples);

void XPT2046_powerDown();

extern uint8_t CTRL_LO_DFR;
extern uint8_t CTRL_LO_SER;
extern uint8_t CTRL_HI_X;
extern uint8_t CTRL_HI_Y;

extern uint16_t ADC_MAX;  // 12 bits

extern uint16_t _width, _height;
extern enum rotation_t _rot;
extern uint8_t _cs_pin, _irq_pin;

extern int32_t _cal_dx, _cal_dy, _cal_dvi, _cal_dvj;
extern uint16_t _cal_vi1, _cal_vj1;

uint16_t XPT2046_readLoop(uint8_t ctrl, uint8_t max_samples);

#endif  // _XPT2046_h_
