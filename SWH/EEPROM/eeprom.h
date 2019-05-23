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
#include "ds1307_mid_level.h"

#define MAX_NUMBER_OF_HISTORIES			10
#define MAX_NUMBER_OF_PAYLOAD_BYTES		(const int)(MAX_NUMBER_OF_HISTORIES * 14 + 3)

typedef union {
	struct {
		unsigned char boilerPump :1;
		unsigned char collectorPump :1;
		unsigned char nRFComm :1;
		unsigned char reserved :5;
	} bits;
	unsigned char B;
} StatusByte_t;

typedef struct {
	TimeStruct_t time;
	StatusByte_t status;
	uint8_t reserved;

	int16_t tempBoiler;
	int16_t tempWaterHeater;
	int16_t tempCollector;
} historyData_t;

typedef union {
	struct {
		historyData_t data[MAX_NUMBER_OF_HISTORIES];
		uint8_t currentIndex;

		uint8_t deltaPlus;
		uint8_t deltaMinus;
	} Item;
	unsigned char Byte[MAX_NUMBER_OF_PAYLOAD_BYTES];
} Payload_t;

typedef struct {
	unsigned char Header;
	Payload_t Payload;
	unsigned char _CRC;
} EEPROM_Array_t;

extern EEPROM_Array_t m_EEPROM_Array;

uint8_t at24c_read(void);
uint8_t at24c_write(void);
void at24c_test(void);

#endif	/* EEPROM_H */

