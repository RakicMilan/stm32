/**
 ******************************************************************************
 * @file		at24c32.c
 * @author		Milan Rakic
 * @date		31 March 2019
 ******************************************************************************
 */

#include <string.h>
#include "stm32f10x_i2c.h"
#include "at24c32.h"
#include "i2c.h"
#include "defines.h"
#include "debugUsart.h"

uint8_t at24c_writeByte(uint16_t addr, uint8_t data) {
	uint8_t tmp[2];
	tmp[0] = (uint8_t)(((unsigned) addr) >> 8);
	tmp[1] = (uint8_t)(((unsigned) addr) & 0xFF);

	i2c_start(I2C2);
	i2c_address_direction(I2C2, AT24C_ADDR << 1, I2C_Direction_Transmitter);
	i2c_transmit(I2C2, tmp[0]);
	i2c_transmit(I2C2, tmp[1]);

	i2c_stop(I2C2);
	i2c_start(I2C2);

	i2c_address_direction(I2C2, AT24C_ADDR << 1, I2C_Direction_Transmitter);
	i2c_transmit(I2C2, data);
	i2c_stop(I2C2);

	return true;
}

uint8_t at24c_readByte(uint16_t addr, uint8_t *data) {
	uint8_t tmp[2];
	tmp[0] = (uint8_t)(((unsigned) addr) >> 8);
	tmp[1] = (uint8_t)(((unsigned) addr) & 0xFF);

	i2c_start(I2C2);
	i2c_address_direction(I2C2, AT24C_ADDR << 1, I2C_Direction_Transmitter);
	i2c_transmit(I2C2, tmp[0]);
	i2c_transmit(I2C2, tmp[1]);

	i2c_stop(I2C2);
	i2c_start(I2C2);

	i2c_address_direction(I2C2, AT24C_ADDR << 1, I2C_Direction_Receiver);
	*data = i2c_receive_nack(I2C2);
	i2c_stop(I2C2);

	return true;
}

// write within a page
// note if you try to write past a page boundary the write will
// wrap back to the start of the same page, so you need to know
// how much you're writing and where you're writing it to
// you don't need to start writing at the start of a page, but if you
// start in the middle you'll be able to write less before wrapping
// optionally wait for the eeprom to complete the write
// returns true to indicate success
uint8_t at24c_writeInPage(uint16_t addr, uint8_t* data, uint8_t len) {
	uint8_t loop;

	// set data address (includes i2c setup,
	// so no need to call i2c_master_start here)
	//if (!at24c_setAddr(addr)) return false;
	uint8_t tmp[2];
	tmp[0] = (uint8_t)(((unsigned) addr) >> 8);
	tmp[1] = (uint8_t)(((unsigned) addr) & 0xFF);

	i2c_start(I2C2);
	i2c_address_direction(I2C2, AT24C_ADDR << 1, I2C_Direction_Transmitter);
	i2c_transmit(I2C2, tmp[0]);
	i2c_transmit(I2C2, tmp[1]);

	i2c_stop(I2C2);
	i2c_start(I2C2);

	i2c_address_direction(I2C2, AT24C_ADDR << 1, I2C_Direction_Transmitter);

	// send the data
	for (loop = 0; loop < len; loop++) {
		i2c_transmit(I2C2, data[loop]);
	}

	// signal i2c stop
	i2c_stop(I2C2);

	return true;
}

// writes across pages
// you do not need to worry about how long your data is or where you
// are writing it as it will be written in multiple parts across
// successive pages, optionally waiting for the last write to complete
// (we always have to wait for any earlier writes to complete)
// note: does not check if you are trying to write past the end of the
// eeprom!
// returns true to indicate success
uint8_t at24c_writeAcrossPages(uint16_t addr, uint8_t* data, uint16_t len) {
	uint8_t wlen;

	// work out number of bytes available in starting page
	wlen = AT24C_PAGESIZE - (addr % AT24C_PAGESIZE);
	// is that more than we actually need?
	if (wlen > len) wlen = len;

	while(wlen > 0) {
		// reduce remaining length
		len -= wlen;
		// write the page
		if (!at24c_writeInPage(addr, data, wlen)) {
			return false;
		}
		// advance the eeprom address and our data pointer
		addr += wlen;
		data += wlen;
		// work out how much to write next time
		wlen = (len < AT24C_PAGESIZE ? len : AT24C_PAGESIZE);
	}

	return true;
}

