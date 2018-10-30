/**
 ******************************************************************************
 * @file		i2c.c
 * @author		Milan Rakic
 * @date		21 September 2018
 ******************************************************************************
 */

/** Includes ---------------------------------------------------------------- */
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_i2c.h"
#include "i2c.h"

/** Public functions ------------------------------------------------------- */
/**
 ******************************************************************************
 *	@brief	Initialize I2C1 in master mode
 * @param	None
 * @retval	None
 ******************************************************************************
 */
void init_i2c1_master(void) {
	// Initialization struct
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;

	// Initialize GPIO as open drain alternate function
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* I2C1 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed = 100000 * 4;
	I2C_Init(I2C1, &I2C_InitStruct);
	I2C_Cmd(I2C1, ENABLE);
}

/**
 ******************************************************************************
 *	@brief	Initialize I2C2 in master mode
 * @param	None
 * @retval	None
 ******************************************************************************
 */
void init_i2c2_master(void) {
	// Initialization struct
	I2C_InitTypeDef I2C_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;

	// Initialize GPIO as open drain alternate function
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// Initialize I2C
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Disable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed = 100000;
	I2C_Init(I2C2, &I2C_InitStruct);
	I2C_Cmd(I2C2, ENABLE);
}

/**
 ******************************************************************************
 *	@brief	Generate I2C start condition
 * @param   I2Cx: where x can be 1 or 2 to select the I2C peripheral.
 * @retval	None
 ******************************************************************************
 */
void i2c_start(I2C_TypeDef* I2Cx) {
	// Wait until I2Cx is not busy anymore
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
		;

	// Generate start condition
	I2C_GenerateSTART(I2Cx, ENABLE);

	// Wait for I2C EV5. 
	// It means that the start condition has been correctly released 
	// on the I2C bus (the bus is free, no other devices is communicating))
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
		;
}

/**
 ******************************************************************************
 *	@brief	Generate I2C stop condition
 * @param   I2Cx: where x can be 1 or 2 to select the I2C peripheral.
 * @retval	None
 ******************************************************************************
 */
void i2c_stop(I2C_TypeDef* I2Cx) {
	// Generate I2C stop condition
	I2C_GenerateSTOP(I2Cx, ENABLE);
}

/**
 ******************************************************************************
 *	@brief	Write slave address to I2C bus
 * @param   I2Cx: where x can be 1 or 2 to select the I2C peripheral.
 * @param	Slave address
 * @param	I2C direction (transmitter or receiver)
 * @retval	None
 ******************************************************************************
 */
void i2c_address_direction(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction) {
	// Send slave address
	I2C_Send7bitAddress(I2Cx, address, direction);

	// Wait for I2C EV6
	// It means that a slave acknowledges his address
	if (direction == I2C_Direction_Transmitter) {
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
			;
	} else if (direction == I2C_Direction_Receiver) {
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
			;
	}
}

/**
 ******************************************************************************
 *	@brief	Transmit one byte to I2C bus
 * @param   I2Cx: where x can be 1 or 2 to select the I2C peripheral.
 * @param	Data byte to transmit
 * @retval	None
 ******************************************************************************
 */
void i2c_transmit(I2C_TypeDef* I2Cx, uint8_t byte) {
	// Send data byte
	I2C_SendData(I2Cx, byte);
	// Wait for I2C EV8_2.
	// It means that the data has been physically shifted out and 
	// output on the bus)
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		;
}

/**
 ******************************************************************************
 *	@brief	Receive data byte from I2C bus, then return ACK
 * @param   I2Cx: where x can be 1 or 2 to select the I2C peripheral.
 * @retval	Received data byte
 ******************************************************************************
 */
uint8_t i2c_receive_ack(I2C_TypeDef* I2Cx) {
	// Enable ACK of received data
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	// Wait for I2C EV7
	// It means that the data has been received in I2C data register
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))
		;

	// Read and return data byte from I2C data register
	return I2C_ReceiveData(I2Cx);
}

/**
 ******************************************************************************
 *	@brief	Receive data byte from I2C bus, then return NACK
 * @param   I2Cx: where x can be 1 or 2 to select the I2C peripheral.
 * @retval	Received data byte
 ******************************************************************************
 */
uint8_t i2c_receive_nack(I2C_TypeDef* I2Cx) {
	// Disable ACK of received data
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	// Wait for I2C EV7
	// It means that the data has been received in I2C data register
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))
		;

	// Read and return data byte from I2C data register
	return I2C_ReceiveData(I2Cx);
}

