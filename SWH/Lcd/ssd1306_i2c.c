/**
 ******************************************************************************
 * @file		ssd1306_i2c.c
 * @author		Milan Rakic
 * @date		21 September 2018
 ******************************************************************************
 */

/** Includes ---------------------------------------------------------------- */
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"

#include "ssd1306_i2c.h"
#include "i2c.h"

/** Public functions -------------------------------------------------------- */
void ssd1306_Reset(void) {
	/* for I2C - do nothing */
}

// Send a byte to the command register
void ssd1306_WriteCommand(uint8_t byte) {
	i2c_start(SSD1306_I2Cx);
	i2c_address_direction(SSD1306_I2Cx, SSD1306_I2C_ADDR, I2C_Direction_Transmitter);
	i2c_transmit(SSD1306_I2Cx, 0x00); // some use 0X00 other examples use 0X80. I tried both
	i2c_transmit(SSD1306_I2Cx, byte);
	i2c_stop(SSD1306_I2Cx);
}

// Send data
void ssd1306_WriteData(uint8_t* buffer, uint16_t buff_size) {
	uint16_t i;
	i2c_start(SSD1306_I2Cx);
	i2c_address_direction(SSD1306_I2Cx, SSD1306_I2C_ADDR, I2C_Direction_Transmitter);
	i2c_transmit(SSD1306_I2Cx, 0X40); // This byte is DATA
	for (i = 0; i < buff_size; i++) {
		i2c_transmit(SSD1306_I2Cx, buffer[i]);
	}
	i2c_stop(SSD1306_I2Cx);
}

