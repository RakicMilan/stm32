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

