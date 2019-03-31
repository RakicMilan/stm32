/**
 ******************************************************************************
 * @file		at24c04.h
 * @author		Milan Rakic
 * @date		31 March 2019
 ******************************************************************************
 */

#ifndef __AT24C04_H__
#define __AT24C04_H__

#include "stm32f10x.h"

// The 24c04 can be treated as two 256 byte eeproms on addresses 50 and 51
#define EEPROM_ADDRESS		0xA0
#define ACKBIT				0x00            // ACK bit
#define NAKBIT				0x80            // NAK bit

uint8_t EEPROMGet(void);
uint8_t EEPROMPut(void);

#endif

