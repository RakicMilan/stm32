/**
 ******************************************************************************
 * @file    eeprom.c
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    29-October-2018
 * @brief   library for communication with eeprom memory.
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#include <string.h>
#include "eeprom.h"
#include "at24c32.h"
#include "defines.h"
#include "debugUsart.h"

//variables
EEPROM_Array_t m_EEPROM_Array;

uint8_t at24c_read(void) {
	uint16_t tmpIndex;
	uint16_t tmpAddress = 0x00;
	unsigned char tmpCRC, tmpHeader;

	memset(m_EEPROM_Array.Payload.Byte, 0x00, MAX_NUMBER_OF_PAYLOAD_BYTES);

//	debug.printf("EEPROMGet: Reading Data...\r\n");
	if (!at24c_readByte(tmpAddress, &m_EEPROM_Array.Header)) { //read header byte
		DebugChangeColorToRED();
		debug.printf("EEPROMGet: Error I2C\r\n");
		DebugChangeColorToGREEN();
		return false;
	}
//	debug.printf("Header:%02X\r\n", m_EEPROM_Array.Header);
	tmpCRC = m_EEPROM_Array.Header;
	tmpAddress++;
	tmpIndex = 0;
	while (tmpIndex < MAX_NUMBER_OF_PAYLOAD_BYTES) {
		at24c_readByte(tmpAddress, &m_EEPROM_Array.Payload.Byte[tmpIndex]); //read 1 byte
		tmpCRC ^= m_EEPROM_Array.Payload.Byte[tmpIndex];
//		debug.printf("%02d:%02X\r\n", tmpIndex,
//				m_EEPROM_Array.Payload.Byte[tmpIndex]);
		tmpIndex++;
		tmpAddress++;
	}
	//read crc
	at24c_readByte(tmpAddress, &m_EEPROM_Array._CRC); //read 1 byte
//	debug.printf("CRC:   %d\r\n", m_EEPROM_Array._CRC);

	tmpCRC = tmpCRC ^ m_EEPROM_Array._CRC;

	tmpHeader = m_EEPROM_Array.Header;
	if ((0x5A == tmpHeader) && (tmpCRC == 0)) {
		debug.printf("EEPROMGet: Format OK\r\n");
		return true;
	} else {
		DebugChangeColorToRED();
		debug.printf("EEPROMGet: Bad format\r\n");
		DebugChangeColorToGREEN();
		return false;
	}
}

uint8_t at24c_write(void) {
	uint16_t tmpIndex;
	uint16_t tmpAddress = 0x00;

	m_EEPROM_Array.Header = 0x5A;
	m_EEPROM_Array._CRC = 0x5A;

//	debug.printf("EEPROMPut: Writing Data..\r\n");
	if (!at24c_writeAcrossPages(tmpAddress, &m_EEPROM_Array.Header, 1, true)) { //write header byte
		DebugChangeColorToRED();
		debug.printf("EEPROMPut: Error I2C\r\n");
		DebugChangeColorToGREEN();
		return false;
	}
//	debug.printf("Header:%02X\r\n", m_EEPROM_Array.Header);
	tmpAddress++;

	// Calc CRC
	tmpIndex = 0x00;
	while (tmpIndex < MAX_NUMBER_OF_PAYLOAD_BYTES) {
		m_EEPROM_Array._CRC ^= m_EEPROM_Array.Payload.Byte[tmpIndex];
//		debug.printf("%02d:%02X\r\n", tmpIndex,
//				m_EEPROM_Array.Payload.Byte[tmpIndex]);
		tmpIndex++;
	}

	at24c_writeAcrossPages(tmpAddress, m_EEPROM_Array.Payload.Byte,
			MAX_NUMBER_OF_PAYLOAD_BYTES, true);
	tmpAddress += MAX_NUMBER_OF_PAYLOAD_BYTES;

	at24c_writeAcrossPages(tmpAddress, &m_EEPROM_Array._CRC, 1, true); //write CRC byte
//	debug.printf("CRC:   %d\r\n", m_EEPROM_Array._CRC);
	return true;
}

void at24c_test(void) {
	uint16_t tmpIndex;
	for (tmpIndex = 0; tmpIndex < MAX_NUMBER_OF_PAYLOAD_BYTES; tmpIndex++) {
		m_EEPROM_Array.Payload.Byte[tmpIndex] = tmpIndex;
	}
	at24c_write();
	at24c_read();
}

