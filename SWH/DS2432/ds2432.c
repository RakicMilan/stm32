/**
 ******************************************************************************
 * @file    ds2432.c
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    21-August-2018
 * @brief   DS2432 EEPROM library.
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <string.h>
#include "ds2432.h"
#include "oneWire.h"
#include "defines.h"
#include "systemTicks.h"

unsigned char DS2432_ReadPage(unsigned char page, unsigned char *page_data, unsigned char size) {
	unsigned char i;

	// select the device
	if (OW_reset(DS2432_PORT, DS2432_PIN)) // Reset the 1-Wire bus
	{
		return false; // Return if no devices found
	}

	OW_writeByte(DS2432_PORT, DS2432_PIN, OW_CMD_SKIPROM); // Send Skip ROM command to select single device
	OW_writeByte(DS2432_PORT, DS2432_PIN, OW_CMD_SEARCHROM); // Read Authentication command
	OW_writeByte(DS2432_PORT, DS2432_PIN, (page << 5) & 0xFF); // TA1
	OW_writeByte(DS2432_PORT, DS2432_PIN, 0x00); // TA2 (always zero for DS2432)

	_DelayUS(100);

	// read the page data
	for (i = 0; i < (size > PAGE_SIZE ? PAGE_SIZE : size); i++) {
		page_data[i] = OW_readByte(DS2432_PORT, DS2432_PIN);
//		debug.printf("DATA[%d]: %d\r\n", i, page_data[i]);
	}

	return true;
}

unsigned char DS2432_WritePage(unsigned char page, unsigned char *page_data, unsigned char numOfRows) {
	unsigned char Oid[8];
	unsigned char i, rowNum, TA1, TA2, E_S;
	unsigned char rstatus[8];
	unsigned char addressMSB;

	addressMSB = (page << 5) & 0xFF;

	for (rowNum = 0; rowNum < numOfRows; rowNum++) {
		// select the device
		if (OW_reset(DS2432_PORT, DS2432_PIN)) // Reset the 1-Wire bus
		{
			return false; // Return if no devices found
		}

		OW_writeByte(DS2432_PORT, DS2432_PIN, OW_CMD_SKIPROM); // Send Skip ROM command to select single device
		OW_writeByte(DS2432_PORT, DS2432_PIN, 0x0F); // Read Authentication command
		OW_writeByte(DS2432_PORT, DS2432_PIN, addressMSB);
		OW_writeByte(DS2432_PORT, DS2432_PIN, 0x00);

		for (i = 0; i < 8; i++) {
			OW_writeByte(DS2432_PORT, DS2432_PIN, page_data[i + (rowNum * 8)]);
		}
		//crc
		rstatus[0] = OW_readByte(DS2432_PORT, DS2432_PIN);
		rstatus[1] = OW_readByte(DS2432_PORT, DS2432_PIN);

		_DelayUS(200);

		// select the device
		if (OW_reset(DS2432_PORT, DS2432_PIN)) // Reset the 1-Wire bus
		{
			return false; // Return if no devices found
		}
		OW_writeByte(DS2432_PORT, DS2432_PIN, OW_CMD_SKIPROM); // Send Skip ROM command to select single device
		OW_writeByte(DS2432_PORT, DS2432_PIN, 0xAA);
		TA1 = OW_readByte(DS2432_PORT, DS2432_PIN);
		TA2 = OW_readByte(DS2432_PORT, DS2432_PIN);
		E_S = OW_readByte(DS2432_PORT, DS2432_PIN);

		_DelayUS(10);
		for (i = 0; i < 8; i++) {
			Oid[i] = OW_readByte(DS2432_PORT, DS2432_PIN);
		}
		//crc
		rstatus[2] = OW_readByte(DS2432_PORT, DS2432_PIN);
		rstatus[3] = OW_readByte(DS2432_PORT, DS2432_PIN);

		// select the device
		if (OW_reset(DS2432_PORT, DS2432_PIN)) // Reset the 1-Wire bus
		{
			return false; // Return if no devices found
		}
		OW_writeByte(DS2432_PORT, DS2432_PIN, OW_CMD_SKIPROM); // Send Skip ROM command to select single device
		OW_writeByte(DS2432_PORT, DS2432_PIN, OW_CMD_MATCHROM); // Read Authentication command
		OW_writeByte(DS2432_PORT, DS2432_PIN, addressMSB);
		OW_writeByte(DS2432_PORT, DS2432_PIN, 0x00); // TA2 (always zero for DS2432)
		OW_writeByte(DS2432_PORT, DS2432_PIN, 0x07);

		_DelayUS(15000);
		//Wait tPROGMAX for the copy function to complete
		rstatus[4] = OW_readByte(DS2432_PORT, DS2432_PIN);

		Oid[2] = TA1;
		Oid[3] = TA2;
		Oid[0] = E_S;
		Oid[4] = rstatus[4];

		addressMSB += 8;
	}

	return true;
}

/**
  * @brief  Initializes the one-wire communication for DS2432
  * @retval None
  */
void DS2432_Init(void) {
	GPIO_InitTypeDef GPIO_OneWireInitStruct;
	GPIO_OneWireInitStruct.GPIO_Pin = DS2432_PIN;
	GPIO_OneWireInitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_OneWireInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS2432_PORT, &GPIO_OneWireInitStruct);
}

unsigned char DS2432_ReadData(unsigned char *page_data, unsigned char size) {
	if (size <= PAGE_SIZE) {
		return DS2432_ReadPage(0, page_data, size);
	} else {
		unsigned char pageIndex;
		unsigned char pageNumbers;
		if (size % PAGE_SIZE == 0) {
			pageNumbers = size / PAGE_SIZE;
		} else {
			pageNumbers = (size / PAGE_SIZE) + 1;
		}
		if (pageNumbers > PAGE_NUMBERS) {
			pageNumbers = PAGE_NUMBERS;
		}
		for (pageIndex = 0; pageIndex < pageNumbers; pageIndex++) {
			unsigned char readSize;
			if ((pageIndex == pageNumbers - 1) && (size % PAGE_SIZE != 0)) {
				readSize = size % PAGE_SIZE;
			} else {
				readSize = PAGE_SIZE;
			}
			unsigned char readData[readSize];
			if (!DS2432_ReadPage(pageIndex, readData, readSize)) {
				return false;
			}
			memcpy(page_data + (pageIndex * PAGE_SIZE), readData, readSize);
		}
		return true;
	}
}

unsigned char DS2432_WriteData(unsigned char *page_data, unsigned char size) {
	if (size <= PAGE_SIZE) {
		return DS2432_WritePage(0, page_data, size / 8);
	} else {
		unsigned char pageIndex;
		unsigned char pageNumbers;
		if (size % PAGE_SIZE == 0) {
			pageNumbers = size / PAGE_SIZE;
		} else {
			pageNumbers = (size / PAGE_SIZE) + 1;
		}
		if (pageNumbers > PAGE_NUMBERS) {
			pageNumbers = PAGE_NUMBERS;
		}
		for (pageIndex = 0; pageIndex < pageNumbers; pageIndex++) {
			unsigned char writeSize;
			if ((pageIndex == pageNumbers - 1) && (size % PAGE_SIZE != 0)) {
				writeSize = size % PAGE_SIZE;
			} else {
				writeSize = PAGE_SIZE;
			}
			unsigned char writeData[writeSize];
			memcpy(writeData, page_data + (pageIndex * PAGE_SIZE), writeSize);
			if (!DS2432_WritePage(pageIndex, writeData, writeSize / 8)) {
				return false;
			}
		}
		return true;
	}
}

