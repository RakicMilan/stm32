/**
 ******************************************************************************
 * @file		at24c32.h
 * @author		Milan Rakic
 * @date		31 March 2019
 ******************************************************************************
 */

#ifndef __AT24C32_H__
#define __AT24C32_H__

#include "stm32f10x.h"

// should work with all atmel 24c series i2c eeproms up to 512Kb
// the 1Mb model uses a 17 bit address, where the most significant
// bit takes the place of the last bit in the device i2c address
// (so it only has A0-A1 for addressing), this driver will work
// fine on this chip but will only be able to access half of it
// (which half depends on the least significant bit of the address
// specified below)

// address can be from 0x50-0x57 depending on address pins A0-A2
#define AT24C_ADDR 0x57
#define AT24C_PAGESIZE 0x20

uint8_t at24c_writeByte(uint16_t addr, uint8_t data);
uint8_t at24c_readByte(uint16_t addr, uint8_t *data);

#endif

