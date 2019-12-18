/*
 * Copyright (c) 2015-2016  Spiros Papadimitriou
 *
 * This file is part of github.com/spapadim/XPT2046 and is released
 * under the MIT License: https://opensource.org/licenses/MIT
 *
 * This software is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied.
 */

#include "XPT2046.h"

uint16_t CAL_MARGIN = 20;
uint8_t CTRL_LO_DFR = 0b0011;
uint8_t CTRL_LO_SER = 0b0100;
uint8_t CTRL_HI_X = 0b1001  << 4;
uint8_t CTRL_HI_Y = 0b1101  << 4;

uint16_t ADC_MAX = 0x0fff;  // 12 bits

uint16_t _width, _height;
enum rotation_t _rot;
uint8_t _cs_pin, _irq_pin;

int32_t _cal_dx, _cal_dy, _cal_dvi, _cal_dvj;
uint16_t _cal_vi1, _cal_vj1;

inline static void swap(uint16_t *a, uint16_t *b) 
{
  uint16_t tmp = *a;
  *a = *b;
  *b = tmp;
}

/**********************************************************/
/*#if 0

// Bisection-based median; will modify vals array
static uint16_t fastMedian (uint16_t *vals, uint8_t n) {
  uint8_t l = 0, r = n;

  while (l < r) {
    uint16_t pivot = vals[l];  // TODO use middle elt?
    uint8_t i = l+1, j = r-1;
    while (i <= j) {
      while ((i < r) && (vals[i] <= pivot)) {
        ++i;
      }
      while ((j > l) && (vals[j] > pivot)) {
        --j;
      }
      if (i < j) {
        swap(vals[i], vals[j]);
      }
    }
    swap(vals[l], vals[j]);

    // j is final pivot position
    if (j == n/2) {
      return vals[j];
    } else if (n/2 < j) {
      r = i;
    } else { // n/2 > j
      l = j+1;
    }
  }
}

static uint16_t mean (const uint16_t *vals, uint8_t n) {
  uint32_t sum = 0;
  for (uint8_t i = 0;  i < n;  i++) {
    sum += vals[i];
  }
  return (uint16_t)(sum/n);
}

#endif // 0*/
/**********************************************************/

void XPT2046_Init(uint16_t width, uint16_t height) 
{
	XPT2046_DDR_MOSI |= (1<<XPT2046_BIT_MOSI);	//MOSI set as output
	XPT2046_DDR_MISO &= ~(1<<XPT2046_BIT_MISO);	//MISO set as input
	XPT2046_PORT_MISO &= ~(1<<XPT2046_BIT_MISO);	//pull-up on MISO off
	XPT2046_DDR_SCK |= (1<<XPT2046_BIT_SCK);	//SCK set as output
	XPT2046_DDR_CS |= (1<<XPT2046_BIT_CS);	//CS set as output
	
#ifndef SHARE_SPI
	XPT2046_SPCR = XPT2046_SPCR_value;
	XPT2046_SPSR = XPT2046_SPSR_value;
#endif

  _width = width;
  _height = height;

  XPT2046_setRotation(ROT90);
  XPT2046_setCalibration(320,180,1885,1765);
  XPT2046_powerDown();  // Make sure PENIRQ is enabled
}

uint8_t XPT2046_SPIwrite(uint8_t c) 
{
#ifndef SHARE_SPI
	uint8_t result=0;

	XPT2046_SPDR = c;
    while(!(XPT2046_SPSR & (1<<SPIF)));
	result = XPT2046_SPDR;
	return result;
#else
	uint8_t result=0;
    uint8_t backupSPCR = XPT2046_SPCR;
    uint8_t backupSPSR = XPT2046_SPSR;
	
    XPT2046_SPCR = XPT2046_SPCR_value;
    XPT2046_SPSR = XPT2046_SPSR_value;
	XPT2046_SPDR = c;
    while(!(XPT2046_SPSR & (1<<SPIF0)));
	result = XPT2046_SPDR;
    XPT2046_SPCR = backupSPCR;
    XPT2046_SPSR = backupSPSR;
	return result;

#endif	//SHARE_SPI
}

uint16_t XPT2046_SPIwrite16(uint16_t c) 
{
#ifndef SHARE_SPI
	union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } in, out;
	in.val=c;

	if (!(XPT2046_SPCR & (1<<DORD)))
	{
		XPT2046_SPDR = in.msb;
		while(!(XPT2046_SPSR & (1<<SPIF)));
		out.msb = XPT2046_SPDR;
		XPT2046_SPDR = in.lsb;
		while(!(XPT2046_SPSR & (1<<SPIF)));
		out.lsb = XPT2046_SPDR;
	}
	else
	{
		XPT2046_SPDR = in.lsb;
		while(!(XPT2046_SPSR & (1<<SPIF)));
		out.lsb = XPT2046_SPDR;
		XPT2046_SPDR = in.msb;
		while(!(XPT2046_SPSR & (1<<SPIF)));
		out.msb = XPT2046_SPDR;
	}

	return out.val;
#else
	union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } in, out;
	uint8_t backupSPCR = XPT2046_SPCR;
	uint8_t backupSPSR = XPT2046_SPSR;

	in.val=c;
	XPT2046_SPCR = XPT2046_SPCR_value;
	XPT2046_SPSR = XPT2046_SPSR_value;
	if (!(XPT2046_SPCR & (1<<DORD0)))
	{
		XPT2046_SPDR = in.msb;
		while(!(XPT2046_SPSR & (1<<SPIF0)));
		out.msb = XPT2046_SPDR;
		XPT2046_SPDR = in.lsb;
		while(!(XPT2046_SPSR & (1<<SPIF0)));
		out.lsb = XPT2046_SPDR;
	}
	else
	{
		XPT2046_SPDR = in.lsb;
		while(!(XPT2046_SPSR & (1<<SPIF0)));
		out.lsb = XPT2046_SPDR;
		XPT2046_SPDR = in.msb;
		while(!(XPT2046_SPSR & (1<<SPIF0)));
		out.msb = XPT2046_SPDR;
	}
	XPT2046_SPCR = backupSPCR;
	XPT2046_SPSR = backupSPSR;

	return out.val;
#endif	//SHARE_SPI
}
void XPT2046_getCalibrationPoints(uint16_t *x1, uint16_t *y1, uint16_t *x2, uint16_t *y2) {
  *x1 = *y1 = CAL_MARGIN;
  *x2 = _width - CAL_MARGIN;
  *y2 = _height - CAL_MARGIN;
}

void XPT2046_setCalibration (uint16_t vi1, uint16_t vj1, uint16_t vi2, uint16_t vj2) {
  _cal_dx = _width - 2*CAL_MARGIN;
  _cal_dy = _height - 2*CAL_MARGIN;

  _cal_vi1 = (int32_t)vi1;
  _cal_vj1 = (int32_t)vj1;
  _cal_dvi = (int32_t)vi2 - vi1;
  _cal_dvj = (int32_t)vj2 - vj1;
}

void XPT2046_setRotation(enum rotation_t rot) 
{ 
	_rot = rot; 
}
	
uint16_t XPT2046_readLoop(uint8_t ctrl, uint8_t max_samples)
{
  uint16_t prev = 0xffff, cur = 0xffff;
  uint8_t i = 0;
  do {
    prev = cur;
    cur = XPT2046_SPIwrite(0);
    cur = (cur << 4) | (XPT2046_SPIwrite(ctrl) >> 4);  // 16 clocks -> 12-bits (zero-padded at end)
  } while ((prev != cur) && (++i < max_samples));
//Serial.print("RL i: "); Serial.println(i); Serial.flush();  // DEBUG
  return cur;
}

// TODO: Caveat - MODE_SER is completely untested!!
//   Need to measure current draw and see if it even makes sense to keep it as an option
void XPT2046_getRaw (uint16_t *vi, uint16_t *vj, enum adc_ref_t mode, uint8_t max_samples)
{
  // Implementation based on TI Technical Note http://www.ti.com/lit/an/sbaa036/sbaa036.pdf

  uint8_t ctrl_lo = ((mode == MODE_DFR) ? CTRL_LO_DFR : CTRL_LO_SER);
  
  XPT2046_CS_SELECT();
  XPT2046_SPIwrite(CTRL_HI_X | ctrl_lo);  // Send first control byte
  *vi = XPT2046_readLoop(CTRL_HI_X | ctrl_lo, max_samples);
  *vj = XPT2046_readLoop(CTRL_HI_Y | ctrl_lo, max_samples);

  if (mode == MODE_DFR) {
    // Turn off ADC by issuing one more read (throwaway)
    // This needs to be done, because PD=0b11 (needed for MODE_DFR) will disable PENIRQ
    XPT2046_SPIwrite(0);  // Maintain 16-clocks/conversion; _readLoop always ends after issuing a control byte
    XPT2046_SPIwrite(CTRL_HI_Y | CTRL_LO_SER);
  }
  XPT2046_SPIwrite16(0);  // Flush last read, just to be sure
  
  XPT2046_CS_DESELECT();
}

uint8_t XPT2046_isTouching() 
{ 
	return (IS_XPT2046_IRQ_LOW()); 
}
	
void XPT2046_getPosition (uint16_t *x, uint16_t *y, enum adc_ref_t mode, uint8_t max_samples)
{
  if (!XPT2046_isTouching()) {
    *x = *y = 0xffff;
    return;
  }

  uint16_t vi, vj;
  int32_t dvi,dvj;
  XPT2046_getRaw(&vi, &vj, mode, max_samples);

  dvi = (int32_t)vi - (int32_t)_cal_vi1;
  if (dvi<0) dvi=0;
  dvj = (int32_t)vj - (int32_t)_cal_vj1;
  if (dvj<0) dvj=0;
  // Map to (un-rotated) display coordinates
#if defined(SWAP_AXES) && SWAP_AXES
  *x = (uint16_t)(_cal_dx * dvj / _cal_dvj + CAL_MARGIN);
  *y = (uint16_t)(_cal_dy * dvi / _cal_dvi + CAL_MARGIN);
#else
  *x = (uint16_t)(_cal_dx * dvi / _cal_dvi + CAL_MARGIN);
  *y = (uint16_t)(_cal_dy * dvj / _cal_dvj + CAL_MARGIN);
#endif

  // Transform based on current rotation setting
  // TODO: Is it worth to do this by tweaking _cal_* values instead?
  switch (_rot) {  // TODO double-check
  case ROT90:
    *x = _width - *x;
    swap(x, y);
    break;
  case ROT180:
    *x = _width - *x;
    *y = _height - *y; 
    break;
  case ROT270:
    *y = _height - *y; 
    swap(x, y);
    break;
  case ROT0:
  default:
    // Do nothing
    break;
  }
}

void XPT2046_powerDown()
{
  XPT2046_CS_SELECT();
  // Issue a throw-away read, with power-down enabled (PD{1,0} == 0b00)
  // Otherwise, ADC is disabled
  XPT2046_SPIwrite(CTRL_HI_Y | CTRL_LO_SER);
  XPT2046_SPIwrite16(0);  // Flush, just to be sure
  XPT2046_CS_DESELECT();
}

