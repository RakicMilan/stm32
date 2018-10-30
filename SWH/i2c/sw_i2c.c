/**
 ******************************************************************************
 * @file		sw_i2c.c
 * @author		Milan Rakic
 * @date		30 October 2018
 ******************************************************************************
 */

/** Includes ---------------------------------------------------------------- */
#include "stm32f10x.h"
#include "sw_i2c.h"
#include "systemTicks.h"

/** Public functions ------------------------------------------------------- */

void init_sw_i2c(void) {
	/* configure pins used by EEPROM
	 * PB6 = SCL
	 * PB7 = SDA
	 */
	GPIO_EepromInitStruct.GPIO_Pin = SCL | SDA;
	GPIO_EepromInitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_EepromInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_PORT, &GPIO_EepromInitStruct);

	I2C_PORT->BSRR = SCL;
	I2C_PORT->BSRR = SDA;
}

/**
 ******************************************************************************
 *	@brief	Generate I2C start condition
 * @retval	None
 ******************************************************************************
 */
void bstart(void) {
	SDA_HIGH(); // Set SDA line
	_DelayUS(2); // Wait a few microseconds
	SCL_HIGH(); // Set SCL line
	_DelayUS(2); // Generate bus free time between Stop
	// and Start condition (Tbuf=4.7us min)
	SDA_LOW(); // Clear SDA line
	_DelayUS(2); // Hold time after (Repeated) Start
	// Condition. After this period, the first clock is generated.
	//(Thd:sta=4.0us min)
	SCL_LOW(); // Clear SCL line
	_DelayUS(2); // Wait a few microseconds
}

/**
 ******************************************************************************
 *	@brief	Generate I2C stop condition
 * @retval	None
 ******************************************************************************
 */
void bstop(void) {
	SCL_LOW(); // Clear SCL line
	_DelayUS(2); // Wait a few microseconds
	SDA_LOW(); // Clear SDA line
	_DelayUS(2); // Wait a few microseconds
	SCL_HIGH(); // Set SCL line
	_DelayUS(2); // Stop condition setup time(Tsu:sto=4.0us min)
	SDA_HIGH(); // Set SDA line
}

/**
 ******************************************************************************
 *	@brief	Transmit one bit to I2C bus
 * @param	Data bit to transmit
 * @retval	None
 ******************************************************************************
 */
void bit_out(unsigned char data) {
	if (data == 0) {
		SDA_LOW();
	} else {
		SDA_HIGH();
	}
	_DelayUS(1);
	SCL_HIGH(); // Set SCL line
	_DelayUS(4); // High Level of Clock Pulse
	SCL_LOW(); // Clear SCL line
	_DelayUS(1); // Low Level of Clock Pulse
	//	mSDA_HIGH();				    // Master release SDA line
}

/**
 ******************************************************************************
 *	@brief	Receive data bit from I2C bus
 * @param   Pointer to received data bit
 * @retval	None
 ******************************************************************************
 */
void bit_in(unsigned char *data) {
//	unsigned char Ack_bit;

	Set_SDA_Input(); // SDA-input
	SCL_HIGH(); // Set SCL line
	_DelayUS(4); // High Level of Clock Pulse
	*data &= 0xFE; // Assume next bit is low
//	Ack_bit = 0;
	if (GPIO_ReadInputDataBit(I2C_PORT, SDA)) { // Check if SDA is high
//		Ack_bit = 1;
		*data |= 0x01; // If high, set next bit
	}
	SCL_LOW(); // Clear SCL line
	_DelayUS(1); // Low Level of Clock Pulse
}

/**
 ******************************************************************************
 *	@brief	Transmit one byte to I2C bus
 * @param	Data byte to transmit
 * @retval	ack
 ******************************************************************************
 */
unsigned char byte_out(unsigned char data) {
	unsigned char i; // Loop counter
	unsigned char ack; // ACK bit

	//ack = 0;

	// Loop through each bit
	for (i = 0; i < 8; i++) {
		if (data & 0x80) {
			ack = 1;
		} else {
			ack = 0;
		}
		bit_out(ack); // Output bit
		data = data << 1; // Shift left for next bit
	}

	bit_in(&ack); // Input ACK bit

	return ack;
}

/**
 ******************************************************************************
 *	@brief	Receive data byte from I2C bus
 * @param   ack
 * @retval	Received data byte
 ******************************************************************************
 */
unsigned char byte_in(unsigned char ack) {
	unsigned char i; // Loop counter
	unsigned char retval; // Return value

	retval = 0;
	for (i = 0; i < 8; i++) // Loop through each bit
			{
		retval = retval << 1; // Shift left for next bit
		bit_in(&retval); // Input bit
	}
	bit_out(ack); // Output ACK/NAK bit

	return retval;
}

