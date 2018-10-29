/**
 ******************************************************************************
 * @file    eeprom.h
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    29-October-2018
 * @brief   library for communication with eeprom memory.
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#ifndef EEPROM_H
#define	EEPROM_H

#include "stm32f10x.h"

// The 24c04 can be treated as two 256 byte eeproms on addresses 50 and 51
#define  EEPROM_ADDRESS             0x50

#define MAX_NUMBER_OF_PAYLOAD_BYTES		(const int)(12)

typedef union {

	struct {
		uint16_t data1;
		uint16_t data2;
		uint16_t data3;
		uint16_t data4;
		uint16_t data5;
		uint16_t data6;
	} Item;
	unsigned char Byte[MAX_NUMBER_OF_PAYLOAD_BYTES];
} Payload_t;

typedef struct {
	unsigned char Header;
	Payload_t Payload;
	unsigned char _CRC;
} EEPROM_Array_t;

extern EEPROM_Array_t m_EEPROM_Array;

extern unsigned char EEPROMGet(void);
extern void EEPROMPut(void);
extern void DebugPrintData(void);

#endif	/* EEPROM_H */

