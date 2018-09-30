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
/**
 ******************************************************************************
 *	@brief	Initialize I2C in master mode
 * @param	None
 * @retval	None
 ******************************************************************************
 */
void ssd1306_i2c_init() {
	// Initialization struct
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;

	// Initialize GPIO as open drain alternate function
	RCC_APB2PeriphClockCmd(SSD1306_I2C_GPIO_RCC, ENABLE);
	GPIO_InitStruct.GPIO_Pin = SSD1306_I2C_PIN_SCL | SSD1306_I2C_PIN_SDA;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SSD1306_I2C_GPIO, &GPIO_InitStruct);

	/* I2C1 clock enable */
	RCC_APB1PeriphClockCmd(SSD1306_I2Cx_RCC, ENABLE);
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed = 100000 * 4;
	I2C_Init(SSD1306_I2Cx, &I2C_InitStruct);
	I2C_Cmd(SSD1306_I2Cx, ENABLE);
}

// Send a byte to the command register
void ssd1306_WriteCommand(uint8_t byte) {
	i2c_start(SSD1306_I2Cx);
	i2c_address_direction(SSD1306_I2Cx, SSD1306_I2C_ADDR,
			I2C_Direction_Transmitter);
	i2c_transmit(SSD1306_I2Cx, 0x00); // some use 0X00 other examples use 0X80. I tried both
	i2c_transmit(SSD1306_I2Cx, byte);
	i2c_stop(SSD1306_I2Cx);
}

// Send data
void ssd1306_WriteData(uint8_t* buffer, uint16_t buff_size) {
	uint16_t i;
	i2c_start(SSD1306_I2Cx);
	i2c_address_direction(SSD1306_I2Cx, SSD1306_I2C_ADDR,
			I2C_Direction_Transmitter);
	i2c_transmit(SSD1306_I2Cx, 0X40); // This byte is DATA
	for (i = 0; i < buff_size; i++) {
		i2c_transmit(SSD1306_I2Cx, buffer[i]);
	}
	i2c_stop(SSD1306_I2Cx);
}

