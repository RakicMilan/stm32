/**
 ******************************************************************************
 * @file		ds1307_i2c.c
 * @author		Milan Rakic
 * @date		21 September 2018
 ******************************************************************************
 */

/** Includes ---------------------------------------------------------------- */
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"

#include "ds1307_i2c.h"
#include "i2c.h"

/** Public functions -------------------------------------------------------- */
/**
 ******************************************************************************
 *	@brief	Write byte to slave without specify register address
 * @param   I2Cx: where x can be 1 or 2 to select the I2C peripheral.
 * @param	Slave device address (7-bit right aligned)
 * @param	Data byte
 * @retval	None
 ******************************************************************************
 */
void i2c_write_no_reg(I2C_TypeDef* I2Cx, uint8_t address, uint8_t data) {
	i2c_start(I2Cx);
	i2c_address_direction(I2Cx, address << 1, I2C_Direction_Transmitter);
	i2c_transmit(I2Cx, data);
	i2c_stop(I2Cx);
}

/**
 ******************************************************************************
 *	@brief	Write byte to slave with specify register address
 * @param   I2Cx: where x can be 1 or 2 to select the I2C peripheral.
 * @param	Slave device address (7-bit right aligned)
 * @param	Register address
 * @param	Data byte
 * @retval	None
 ******************************************************************************
 */
void i2c_write_with_reg(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t data) {
	i2c_start(I2Cx);
	i2c_address_direction(I2Cx, address << 1, I2C_Direction_Transmitter);
	i2c_transmit(I2Cx, reg);
	i2c_transmit(I2Cx, data);
	i2c_stop(I2Cx);
}

/**
 ******************************************************************************
 *	@brief	Write bytes to slave without specify register address where to
 *					start write
 * @param   I2Cx: where x can be 1 or 2 to select the I2C peripheral.
 * @param	Slave device address (7-bit right aligned)
 * @param	Pointer to data byte array
 * @param	Number of bytes to write
 * @retval	None
 ******************************************************************************
 */
void i2c_write_multi_no_reg(I2C_TypeDef* I2Cx, uint8_t address, uint8_t* data, uint8_t len) {
	int i;

	i2c_start(I2Cx);
	i2c_address_direction(I2Cx, address << 1, I2C_Direction_Transmitter);
	for (i = 0; i < len; i++) {
		i2c_transmit(I2Cx, data[i]);
	}
	i2c_stop(I2Cx);
}

/**
 ******************************************************************************
 *	@brief	Write bytes to slave with specify register address where to
 *					start write
 * @param   I2Cx: where x can be 1 or 2 to select the I2C peripheral.
 * @param	Slave device address (7-bit right aligned)
 * @param	Register address where to start write
 * @param	Pointer to data byte array
 * @param	Number of bytes to write
 * @retval	None
 ******************************************************************************
 */
void i2c_write_multi_with_reg(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t* data,
		uint8_t len) {
	int i;

	i2c_start(I2Cx);
	i2c_address_direction(I2Cx, address << 1, I2C_Direction_Transmitter);
	i2c_transmit(I2Cx, reg);
	for (i = 0; i < len; i++) {
		i2c_transmit(I2Cx, data[i]);
	}
	i2c_stop(I2Cx);
}

/**
 ******************************************************************************
 *	@brief	Read byte from slave without specify register address
 * @param   I2Cx: where x can be 1 or 2 to select the I2C peripheral.
 * @param	Slave device address (7-bit right aligned)
 * @param	Pointer to data byte to store data from slave
 * @retval	None
 ******************************************************************************
 */
void i2c_read_no_reg(I2C_TypeDef* I2Cx, uint8_t address, uint8_t* data) {
	i2c_start(I2Cx);
	i2c_address_direction(I2Cx, address << 1, I2C_Direction_Receiver);
	*data = i2c_receive_nack(I2Cx);
	i2c_stop(I2Cx);
}

/**
 ******************************************************************************
 *	@brief	Read byte from slave with specify register address
 * @param   I2Cx: where x can be 1 or 2 to select the I2C peripheral.
 * @param	Slave device address (7-bit right aligned)
 * @param	Register address
 * @param	Pointer to data byte to store data from slave
 * @retval	None
 ******************************************************************************
 */
void i2c_read_with_reg(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t* data) {
	i2c_start(I2Cx);
	i2c_address_direction(I2Cx, address << 1, I2C_Direction_Transmitter);
	i2c_transmit(I2Cx, reg);
	i2c_stop(I2Cx);
	i2c_start(I2Cx);
	i2c_address_direction(I2Cx, address << 1, I2C_Direction_Receiver);
	*data = i2c_receive_nack(I2Cx);
	i2c_stop(I2Cx);
}

/**
 ******************************************************************************
 *	@brief	Read bytes from slave without specify register address
 * @param   I2Cx: where x can be 1 or 2 to select the I2C peripheral.
 * @param	Slave device address (7-bit right aligned)
 * @param	Number of data bytes to read from slave
 * @param	Pointer to data array byte to store data from slave
 * @retval	None
 ******************************************************************************
 */
void i2c_read_multi_no_reg(I2C_TypeDef* I2Cx, uint8_t address, uint8_t len, uint8_t* data) {
	int i;

	i2c_start(I2Cx);
	i2c_address_direction(I2Cx, address << 1, I2C_Direction_Receiver);
	for (i = 0; i < len; i++) {
		if (i == (len - 1)) {
			data[i] = i2c_receive_nack(I2Cx);
		} else {
			data[i] = i2c_receive_ack(I2Cx);
		}
	}
	i2c_stop(I2Cx);
}

/**
 ******************************************************************************
 *	@brief	Read bytes from slave with specify register address
 * @param   I2Cx: where x can be 1 or 2 to select the I2C peripheral.
 * @param	Slave device address (7-bit right aligned)
 * @param	Register address
 * @param	Number of data bytes to read from slave
 * @param	Pointer to data array byte to store data from slave
 * @retval	None
 ******************************************************************************
 */
void i2c_read_multi_with_reg(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t len,
		uint8_t* data) {
	int i;

	i2c_start(I2Cx);
	i2c_address_direction(I2Cx, address << 1, I2C_Direction_Transmitter);
	i2c_transmit(I2Cx, reg);
	i2c_stop(I2Cx);
	i2c_start(I2Cx);
	i2c_address_direction(I2Cx, address << 1, I2C_Direction_Receiver);
	for (i = 0; i < len; i++) {
		if (i == (len - 1)) {
			data[i] = i2c_receive_nack(I2Cx);
		} else {
			data[i] = i2c_receive_ack(I2Cx);
		}
	}
	i2c_stop(I2Cx);
}

