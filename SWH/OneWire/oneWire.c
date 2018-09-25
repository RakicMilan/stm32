/**
 ******************************************************************************
 * @file    oneWire.c
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    21-August-2018
 * @brief   1-Wire library.
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#include <stm32f10x_gpio.h>
#include "oneWire.h"
#include "systemTicks.h"

/**
 * @brief  Generate a 1-Wire reset, return 1 if no presence detect was found, return 0 otherwise.
 *         (NOTE: Does not handle alarm presence from DS2404/DS1994)
 * @param  ow_port: GPIOx port, where x can be (A..G) to select the GPIO peripheral.
 * @param  ow_pin: specifies the port bits to be written.
 *   This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
 * @retval 1 if no presence detect was found, 0 otherwise
 */
int OW_reset(GPIO_TypeDef* ow_port, uint16_t ow_pin) {
	int result;

	_DelayUS(DELAY_G);
	GPIO_ResetBits(ow_port, ow_pin); // Drives DQ low
	_DelayUS(DELAY_H);
	GPIO_SetBits(ow_port, ow_pin); // Releases the bus
	_DelayUS(DELAY_I);
	result = GPIO_ReadInputDataBit(ow_port, ow_pin); // Sample for presence pulse from slave
	_DelayUS(DELAY_J); // Complete the reset sequence recovery
	return result; // Return sample presence pulse result
}

/**
 * @brief  Send a 1-Wire write bit. Provide 10us recovery time.
 * @param  ow_port: GPIOx port, where x can be (A..G) to select the GPIO peripheral.
 * @param  ow_pin: specifies the port bits to be written.
 *   This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
 * @param  bit: bit for writing.
 * @retval None
 */
void OW_writeBit(GPIO_TypeDef* ow_port, uint16_t ow_pin, int bit) {
	if (bit) {
		// Write '1' bit
		GPIO_ResetBits(ow_port, ow_pin); // Drives DQ low
		_DelayUS(DELAY_A);
		GPIO_SetBits(ow_port, ow_pin); // Releases the bus
		_DelayUS(DELAY_B); // Complete the time slot and 10us recovery
	} else {
		// Write '0' bit
		GPIO_ResetBits(ow_port, ow_pin); // Drives DQ low
		_DelayUS(DELAY_C);
		GPIO_SetBits(ow_port, ow_pin); // Releases the bus
		_DelayUS(DELAY_D);
	}
}

/**
 * @brief  Read a bit from the 1-Wire bus and return it. Provide 10us recovery time.
 * @param  ow_port: GPIOx port, where x can be (A..G) to select the GPIO peripheral.
 * @param  ow_pin: specifies the port bits to be written.
 *   This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
 * @retval the bit that's read
 */
int OW_readBit(GPIO_TypeDef* ow_port, uint16_t ow_pin) {
	int result;

	GPIO_ResetBits(ow_port, ow_pin); // Drives DQ low
	_DelayUS(DELAY_A);
	GPIO_SetBits(ow_port, ow_pin); // Releases the bus
	_DelayUS(DELAY_E);
	result = GPIO_ReadInputDataBit(ow_port, ow_pin); // Sample the bit value from the slave
	_DelayUS(DELAY_F); // Complete the time slot and 10us recovery

	return result;
}

/**
 * @brief  Write 1-Wire data byte
 * @param  ow_port: GPIOx port, where x can be (A..G) to select the GPIO peripheral.
 * @param  ow_pin: specifies the port bits to be written.
 *   This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
 * @param  data: data for writing.
 * @retval None
 */
void OW_writeByte(GPIO_TypeDef* ow_port, uint16_t ow_pin, int data) {
	int loop;

	// Loop to write each bit in the byte, LS-bit first
	for (loop = 0; loop < 8; loop++) {
		OW_writeBit(ow_port, ow_pin, data & 0x01);

		// shift the data byte for the next bit
		data >>= 1;
	}
}

/**
 * @brief  Read 1-Wire data byte and return it
 * @param  ow_port: GPIOx port, where x can be (A..G) to select the GPIO peripheral.
 * @param  ow_pin: specifies the port bits to be written.
 *   This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
 * @retval the data that's read
 */
int OW_readByte(GPIO_TypeDef* ow_port, uint16_t ow_pin) {
	int loop, result = 0;

	for (loop = 0; loop < 8; loop++) {
		// shift the result to get it ready for the next bit
		result >>= 1;

		// if result is one, then set MS bit
		if (OW_readBit(ow_port, ow_pin))
			result |= 0x80;
	}
	return result;
}

