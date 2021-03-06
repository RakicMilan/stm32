/**
 ******************************************************************************
 * @file		at24c04.c
 * @author		Milan Rakic
 * @date		31 March 2019
 ******************************************************************************
 */

#include "at24c04.h"
#include "sw_i2c.h"
#include "eeprom.h"
#include "defines.h"
#include "systemTicks.h"
#include "debugUsart.h"

/********************************************************************
 * Function:        void ACK_Poll(void)
 *
 * Description:     This function implements Acknowledge polling.
 *
 * Dependencies:    'control' contains the control byte
 *******************************************************************/
void ACK_Poll(uint8_t argControl) {
	uint8_t tmpResult; // Polling result
	uint8_t tmpRetryCnt = 64;
	tmpResult = 1; // Initialize result

	do {
		bstart(); // Generate Start condition
		if (!tmpRetryCnt--) {
			break;
		}
		_DelayUS(100);
		tmpResult = byte_out(argControl); // Output control byte

	} while (tmpResult == 1);
	bstop(); // Generate Stop condition
}

/********************************************************************
 * Function:        void LowDensByteWrite(unsigned char data)
 *
 * Description:     This function writes a single byte to a
 *                  low-density (<= 16 Kb) serial EEPROM device.
 *
 * Dependencies:    'control' contains the control byte
 *                  'address' contains the address word
 *******************************************************************/
uint8_t LowDensByteWrite(uint8_t argEaddr, uint32_t argAddr, uint8_t data) {
	uint8_t tmpOK = false;
	uint8_t temp_control; // Temp. variable for control byte

	// Merge block bits with control byte
	temp_control = (argEaddr & 0xF1) | ((argAddr >> 7) & 0x0E);

	bstart(); // Generate Start condition
	if (byte_out(temp_control)) { // Output control byte
		return tmpOK;
	}

	if (byte_out(argAddr)) { // Output address LSB
		return tmpOK;
	}
	if (byte_out(data)) { // Output data byte
		return tmpOK;
	}
	tmpOK = true;
	bstop(); // Generate Stop condition
	ACK_Poll(argEaddr); // Begin ACK polling
	return tmpOK;
}

/********************************************************************
 * Function:        void LowDensByteRead(unsigned char *data)
 *
 * Description:     This function reads a single byte from a
 *                  low-density (<= 16 Kb) serial EEPROM device.
 *
 * Dependencies:    'control' contains the control byte
 *                  'address' contains the address word
 *******************************************************************/
uint8_t LowDensByteRead(uint8_t argEaddr, uint32_t argAddr, uint8_t *data) {
	uint8_t tmpOK = false;
	uint8_t temp_control; // Temp. variable for control byte

	// Merge block bits with control byte
	temp_control = (argEaddr & 0xF1) | ((argAddr >> 7) & 0x0E);

	bstart(); // Generate Start condition
	if (byte_out(temp_control)) { // Output control byte
		return tmpOK;
	}
	if (byte_out(argAddr)) { // Output address LSB
		return tmpOK;
	}

	bstart(); // Generate Start condition
	if (byte_out(temp_control | 0x01)) { // Output control byte
		return tmpOK;
	}
	*data = byte_in(NAKBIT); // Input data byte
	tmpOK = true;
	bstop(); // Generate Stop condition
	return tmpOK;
}

uint8_t EEPROMGet(void) {
	uint16_t tmpIndex;
	uint16_t tmpAddress = 0x00;
	unsigned char tmpCRC, tmpHeader;

	debug.printf("EEPROMGet: Reading Data...\r\n");
	if (!LowDensByteRead(EEPROM_ADDRESS, tmpAddress, &m_EEPROM_Array.Header)) { //read header byte
		DebugChangeColorToRED();
		debug.printf("EEPROMGet: Error I2C\r\n");
		DebugChangeColorToGREEN();
		return false;
	}
	debug.printf("Header:%02X\r\n", m_EEPROM_Array.Header);
	tmpCRC = m_EEPROM_Array.Header;
	tmpAddress++;
	tmpIndex = 0;
	while (tmpIndex < MAX_NUMBER_OF_PAYLOAD_BYTES) {
		LowDensByteRead(EEPROM_ADDRESS, tmpAddress,
				&m_EEPROM_Array.Payload.Byte[tmpIndex]); //read 1 byte
		debug.printf("B[%02d]: %02X\r\n", tmpIndex,
				m_EEPROM_Array.Payload.Byte[tmpIndex]);
		tmpCRC ^= m_EEPROM_Array.Payload.Byte[tmpIndex];
		tmpIndex++;
		tmpAddress++;
	}
	//read crc
	LowDensByteRead(EEPROM_ADDRESS, tmpAddress, &m_EEPROM_Array._CRC); //read 1 byte
	debug.printf("CRC:   %d\r\n", m_EEPROM_Array._CRC);

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

uint8_t EEPROMPut(void) {
	uint16_t tmpIndex;
	uint16_t tmpAddress = 0x00;

	m_EEPROM_Array.Header = 0x5A;
	m_EEPROM_Array._CRC = 0x5A;

	debug.printf("EEPROMPut: Writing Data..\r\n");
	if (!LowDensByteWrite(EEPROM_ADDRESS, tmpAddress, m_EEPROM_Array.Header)) { //write header byte
		DebugChangeColorToRED();
		debug.printf("EEPROMPut: Error I2C\r\n");
		DebugChangeColorToGREEN();
		return false;
	}
	debug.printf("Header:%02X\r\n", m_EEPROM_Array.Header);
	tmpAddress++;
	tmpIndex = 0x00;
	while (tmpIndex < MAX_NUMBER_OF_PAYLOAD_BYTES) {
		LowDensByteWrite(EEPROM_ADDRESS, tmpAddress,
				m_EEPROM_Array.Payload.Byte[tmpIndex]); //write 1 byte
		debug.printf("B[%02d]: %02X\r\n", tmpIndex,
				m_EEPROM_Array.Payload.Byte[tmpIndex]);
		m_EEPROM_Array._CRC ^= m_EEPROM_Array.Payload.Byte[tmpIndex];
		tmpIndex++;
		tmpAddress++;
	}
	LowDensByteWrite(EEPROM_ADDRESS, tmpAddress, m_EEPROM_Array._CRC); //write CRC byte
	debug.printf("CRC:   %d\r\n", m_EEPROM_Array._CRC);
	return true;
}

