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
#include "systemTicks.h"

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

// set the current data address, this is the start of the write command
// next either send the data to be written, or start a read instead
// returns true to indicate success
static uint8_t at24c_setAddr(uint16_t addr) {
	uint8_t loop;
	uint8_t data[2];

	// signal i2c start
	i2c_start(I2C2);

	// write i2c address & direction
	i2c_address_direction(I2C2, AT24C_ADDR << 1, I2C_Direction_Transmitter);

	// write data address
	data[0] = (uint8_t)(((unsigned) addr) >> 8);
	data[1] = (uint8_t)(((unsigned) addr) & 0xff);
	for (loop = 0; loop < 2; loop++) {
		i2c_transmit(I2C2, data[loop]);
	}

	return true;
}

// read from the current position
// returns true to indicate success
uint8_t at24c_readNextBytes(uint8_t *data, uint16_t len) {
	int loop;

	// signal i2c start
	i2c_start(I2C2);

	// write i2c address & direction
	i2c_address_direction(I2C2, AT24C_ADDR << 1, I2C_Direction_Receiver);

	// read bytes
	for (loop = 0; loop < len; loop++) {
		if (loop < (len - 1)) {
			data[loop] = i2c_receive_ack(I2C2);
		} else {
			data[loop] = i2c_receive_nack(I2C2);
		}
	}

	// signal i2c stop
	i2c_stop(I2C2);

	return true;
}

// read from anywhere
// sets the address then does a normal read
// returns true to indicate success
uint8_t at24c_readBytes(uint16_t addr, uint8_t *data, uint16_t len) {
	// set data address
	if (!at24c_setAddr(addr))
		return false;
	// perform the read
	return at24c_readNextBytes(data, len);
}

// wait for a write operation to complete
// by 'acknowledge polling'
void at24c_writeWait() {
//	do {
//		i2c_start(I2C2);
//		i2c_master_writeByte((uint8_t)((AT24C_ADDR << 1) | 1));
//	} while (!i2c_master_checkAck());

	i2c_start(I2C2);
	i2c_address_direction(I2C2, AT24C_ADDR << 1, I2C_Direction_Receiver);
	i2c_stop(I2C2);
}

// write within a page
// note if you try to write past a page boundary the write will
// wrap back to the start of the same page, so you need to know
// how much you're writing and where you're writing it to
// you don't need to start writing at the start of a page, but if you
// start in the middle you'll be able to write less before wrapping
// optionally wait for the eeprom to complete the write
// returns true to indicate success
uint8_t at24c_writeInPage(uint16_t addr, uint8_t* data, uint8_t len,
		uint8_t wait) {
	int loop;

	// set data address (includes i2c setup,
	// so no need to call i2c_master_start here)
	if (!at24c_setAddr(addr))
		return false;

	// send the data
	for (loop = 0; loop < len; loop++) {
		i2c_transmit(I2C2, data[loop]);
	}

	// signal i2c stop
	i2c_stop(I2C2);

	// optionally, wait until the eeprom signals the write is finished
//	if (wait)
//		at24c_writeWait();

	_DelayMS(100);

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
uint8_t at24c_writeAcrossPages(uint16_t addr, uint8_t* data, uint16_t len,
		uint8_t wait) {
	uint8_t wlen;

	// work out number of bytes available in starting page
	wlen = AT24C_PAGESIZE - (addr % AT24C_PAGESIZE);
	// is that more than we actually need?
	if (wlen > len)
		wlen = len;

	while (wlen > 0) {
		// reduce remaining length
		len -= wlen;
		// write the page
		if (!at24c_writeInPage(addr, data, wlen, (len > 0 ? true : wait))) {
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

// set an area of eeprom to specified value (like memset)
// optionally wait for completion of last write
uint8_t at24c_setBytes(uint16_t addr, uint8_t val, uint16_t len, uint8_t wait) {
	uint8_t wlen;
	uint8_t data[AT24C_PAGESIZE];

	// set the temp write buffer to user's choice of value
	memset(data, val, AT24C_PAGESIZE);

	// work out number of bytes available in starting page
	wlen = AT24C_PAGESIZE - (addr % AT24C_PAGESIZE);
	// is that more than we actually need?
	if (wlen > len)
		wlen = len;

	while (wlen > 0) {
		// reduce remaining length
		len -= wlen;
		// write the page
		if (!at24c_writeInPage(addr, data, wlen, (len > 0 ? true : wait))) {
			return false;
		}
		// advance the eeprom address
		addr += wlen;
		// work out how much to write next time
		wlen = (len < AT24C_PAGESIZE ? len : AT24C_PAGESIZE);
	}

	return true;
}

