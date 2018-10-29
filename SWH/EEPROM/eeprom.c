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

#include "stm32f10x_i2c.h"
#include "eeprom.h"
#include "i2c.h"
#include "defines.h"
#include "debugUsart.h"
#include "systemTicks.h"

//variables
EEPROM_Array_t m_EEPROM_Array;

/**
 ******************************************************************************
 *	@brief	Write byte to slave with specify register address
 * @param	Register address
 * @param	Data byte
 * @retval	None
 ******************************************************************************
 */
void eeprom_WriteByte(uint16_t argAddr, uint8_t byte) {
	i2c_start(I2C1);
	i2c_address_direction(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);
	i2c_transmit(I2C1, argAddr);
	i2c_transmit(I2C1, byte);
	i2c_stop(I2C1);
}

/**
 ******************************************************************************
 *	@brief	Read byte from slave with specify register address
 * @param	Register address
 * @param	Pointer to data byte to store data from slave
 * @retval	None
 ******************************************************************************
 */
void eeprom_ReadByte(uint16_t argAddr, uint8_t* data) {
	i2c_start(I2C1);
	i2c_address_direction(I2C1, EEPROM_ADDRESS << 1, I2C_Direction_Transmitter);
	i2c_transmit(I2C1, argAddr);
	i2c_stop(I2C1);
	i2c_start(I2C1);
	i2c_address_direction(I2C1, EEPROM_ADDRESS << 1, I2C_Direction_Receiver);
	*data = i2c_receive_nack(I2C1);
	i2c_stop(I2C1);
}

unsigned char EEPROMGet(void) {
	uint16_t tmpIndex;
	uint16_t tmpAddress = 0x00;
	unsigned char tmpCRC, tmpHeader;

	eeprom_ReadByte(tmpAddress, &m_EEPROM_Array.Header);
//	if (!eeprom_ReadByte(tmpAddress, &m_EEPROM_Array.Header)) { //read header byte
//		DebugChangeColorToRED();
//		debug.printf("EEPROMGet: Error I2C\r\n");
//		DebugChangeColorToGREEN();
//	}
	tmpCRC = m_EEPROM_Array.Header;

	tmpAddress++;
	tmpIndex = 0;
	while (tmpIndex < MAX_NUMBER_OF_PAYLOAD_BYTES) {
		eeprom_ReadByte(tmpAddress, &m_EEPROM_Array.Payload.Byte[tmpIndex]); //read 1 byte
		tmpCRC ^= m_EEPROM_Array.Payload.Byte[tmpIndex];
		tmpIndex++;
		tmpAddress++;
	}

	//read crc
	eeprom_ReadByte(tmpAddress, &m_EEPROM_Array._CRC); //read 1 byte

//	debug.printf("EEPROMGet: Reading Data...\r\n");
//	debug.printf("EEPROMGet: Header=%X\r\n", m_EEPROM_Array.Header);
//	for (tmpIndex = 0; tmpIndex < MAX_NUMBER_OF_PAYLOAD_BYTES; tmpIndex++) {
//		debug.printf("D[%d]=%d ", tmpIndex, m_EEPROM_Array.Payload.Byte[tmpIndex]);
//	}
//	debug.printf("\nEEPROMGet: CRC=%d\r\n", m_EEPROM_Array._CRC);

	//check if record is OK
	/*
	 * Because of some problem with C18 compiler and condition processing over data in struct I had to use local variable tmpHeader here
	 */
	tmpCRC = tmpCRC ^ m_EEPROM_Array._CRC;

	tmpHeader = m_EEPROM_Array.Header;
	if ((0x5A == tmpHeader) && (tmpCRC == 0)) {
		debug.printf("EEPROMGet: cal OK\r\n");
		return true;

	} else {
		debug.printf("EEPROMGet: Header=%X\r\n", m_EEPROM_Array.Header);
		debug.printf("\nEEPROMGet: CRC=%d\r\n", m_EEPROM_Array._CRC);
		DebugChangeColorToRED();
		debug.printf("EEPROMGet: Bad config\r\n");
		DebugChangeColorToGREEN();
		return false;
	}
}

void EEPROMPut(void) {
	uint16_t tmpIndex;
	uint16_t tmpAddress = 0x00;
	m_EEPROM_Array.Header = 0x5A;

	m_EEPROM_Array._CRC = 0x5A;

	debug.printf("EEPROMPut: Header=%X\r\n", m_EEPROM_Array.Header);
	debug.printf("EEPROMPut: Writing Data..\r\n");

//	for (tmpIndex = 0; tmpIndex < MAX_NUMBER_OF_PAYLOAD_BYTES; tmpIndex++) {
//		m_EEPROM_Array.Payload.Byte[tmpIndex] = tmpIndex;
//		debug.printf("D[%d]=%d ", tmpIndex, m_EEPROM_Array.Payload.Byte[tmpIndex]);
//	}

	eeprom_WriteByte(tmpAddress, m_EEPROM_Array.Header);
//	if (!eeprom_WriteByte(tmpAddress, m_EEPROM_Array.Header)) { //write header byte
//		DebugChangeColorToRED();
//		debug.printf("EEPROMPut: Error I2C\r\n");
//		DebugChangeColorToGREEN();
//	}
	tmpAddress++;
	tmpIndex = 0x00;
	while (tmpIndex < MAX_NUMBER_OF_PAYLOAD_BYTES) {
		eeprom_WriteByte(tmpAddress, m_EEPROM_Array.Payload.Byte[tmpIndex]); //write 1 byte
		//debug.printf("Byte[%d] %02x\r\n",tmpIndex, m_EEPROM_Array.Payload.Byte[tmpIndex]);
		m_EEPROM_Array._CRC ^= m_EEPROM_Array.Payload.Byte[tmpIndex];
		tmpIndex++;
		tmpAddress++;
	}

	eeprom_WriteByte(tmpAddress, m_EEPROM_Array._CRC); //write CRC byte

	debug.printf("\nEEPROMPut: CRC=%d\r\n", m_EEPROM_Array._CRC);
}

void DebugPrintData(void) {
	unsigned int tmpIndex;
	EEPROMGet();
	debug.printf("DebugPrintData...\r\n");
	debug.printf("Header=%X\r\n", m_EEPROM_Array.Header);
	for (tmpIndex = 0; tmpIndex < MAX_NUMBER_OF_PAYLOAD_BYTES; tmpIndex++) {
		debug.printf("D[%d]=%d ", tmpIndex, m_EEPROM_Array.Payload.Byte[tmpIndex]);
	}
	debug.printf("\r\nCRC=%d\r\n", m_EEPROM_Array._CRC);
}
