/**
 ******************************************************************************
 * @file    nrf24_low_level.c
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    21-August-2018
 * @brief   nRF24 library.
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_spi.h>

#include "nrf24_low_level.h"
#include "debugUsart.h"
#include "systemTicks.h"

// Configure the GPIO lines of the nRF24L01 transceiver
// note: IRQ pin must be configured separately
void nRF24_GPIO_Init(void) {
	GPIO_InitTypeDef PORT;

	// Enable the nRF24L01 GPIO peripherals
	RCC->APB2ENR |= nRF24_GPIO_PERIPHERALS;

	// Configure nRF24 IRQ pin
	PORT.GPIO_Pin = nRF24_IRQ_PIN;
	PORT.GPIO_Mode = GPIO_Mode_IPU;
	PORT.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(nRF24_IRQ_PORT, &PORT);

	// Configure CSN pin
	PORT.GPIO_Mode = GPIO_Mode_Out_PP;
	PORT.GPIO_Speed = GPIO_Speed_50MHz;
	PORT.GPIO_Pin = nRF24_CSN_PIN;
	GPIO_Init(nRF24_CSN_PORT, &PORT);
	nRF24_CSN_H();

	// Configure CE pin
	PORT.GPIO_Pin = nRF24_CE_PIN;
	GPIO_Init(nRF24_CE_PORT, &PORT);
	nRF24_CE_L();
}

/**
 * This funcion initializes the SPI1 peripheral.
 * Setup SPI1 pins A4/SS, A5/SCK, A6/MISO, A7/MOSI on port A.
 * Hardware slave select is not used. That is indicated in the settings.
 */
void Init_SPI1_Master(void) {
	GPIO_InitTypeDef PORT;
	SPI_InitTypeDef SPI;

	// initialize init structs
	GPIO_StructInit(&PORT);
	SPI_StructInit(&SPI);

	// initialize clocks
	RCC_APB2PeriphClockCmd(
			RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA,
			ENABLE);

	// Configure SPI pins (SPI1)
	PORT.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	PORT.GPIO_Mode = GPIO_Mode_AF_PP;
	PORT.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &PORT);

	// initialize A6/MISO input pull-up (50MHz)
	PORT.GPIO_Pin = GPIO_Pin_6;
	PORT.GPIO_Mode = GPIO_Mode_IPU;
	PORT.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &PORT);

	// initialize SPI master
	// for slave, no need to define SPI_BaudRatePrescaler
	SPI.SPI_Mode = SPI_Mode_Master;
	SPI.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI.SPI_CPOL = SPI_CPOL_Low;
	SPI.SPI_CPHA = SPI_CPHA_1Edge;
	SPI.SPI_CRCPolynomial = 10;
	SPI.SPI_DataSize = SPI_DataSize_8b; // 8-bit transactions
	SPI.SPI_FirstBit = SPI_FirstBit_MSB; // MSB first
	SPI.SPI_NSS = SPI_NSS_Soft; // use software SS
	SPI_Init(nRF24_SPI_PORT, &SPI);
	SPI_NSSInternalSoftwareConfig(nRF24_SPI_PORT, SPI_NSSInternalSoft_Set);
	SPI_Cmd(nRF24_SPI_PORT, ENABLE);
}

/**
 * This funcion initializes the SPI1 peripheral.
 * Setup SPI2 pins B12/SS, B13/SCK, B14/MISO, B15/MOSI on port B
 * Hardware slave select is not used. That is indicated in the settings.
 */
void Init_SPI2_Master(void) {
	GPIO_InitTypeDef PORT;
	SPI_InitTypeDef SPI;

	// initialize init structs
	GPIO_StructInit(&PORT);
	SPI_StructInit(&SPI);

	// Enable SPI2 peripheral
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	// Enable SPI2 GPIO peripheral (PORTB)
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	// initialize clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);

	// Configure SPI pins (SPI2)
	PORT.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	PORT.GPIO_Mode = GPIO_Mode_AF_PP;
	PORT.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &PORT);

	// initialize B14/MISO input pull-up (50 MHz)
	PORT.GPIO_Pin = GPIO_Pin_14;
	PORT.GPIO_Mode = GPIO_Mode_IPU;
	PORT.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &PORT);

	// Initialize SPI2
	SPI.SPI_Mode = SPI_Mode_Master;
	SPI.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI.SPI_CPOL = SPI_CPOL_Low;
	SPI.SPI_CPHA = SPI_CPHA_1Edge;
	SPI.SPI_CRCPolynomial = 10;
	SPI.SPI_DataSize = SPI_DataSize_8b;
	SPI.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(nRF24_SPI_PORT, &SPI);
	SPI_NSSInternalSoftwareConfig(nRF24_SPI_PORT, SPI_NSSInternalSoft_Set);
	SPI_Cmd(nRF24_SPI_PORT, ENABLE);
}

/**
 * Low level SPI transmit/receive function (hardware depended)
 * Transfer a byte over SPI2 B12/SS, B13/SCK, B14/MISO, B15/MOSI.
 * input:
 *   data - value to transmit via SPI
 * return: value received from SPI
 */
uint8_t nRF24_LL_RW(uint8_t data) {
	// Wait until TX buffer is empty
	while (SPI_I2S_GetFlagStatus(nRF24_SPI_PORT, SPI_I2S_FLAG_TXE) == RESET)
		;
	// Send byte to SPI (TXE cleared)
	SPI_I2S_SendData(nRF24_SPI_PORT, data);
	// Wait while receive buffer is empty
	while (SPI_I2S_GetFlagStatus(nRF24_SPI_PORT, SPI_I2S_FLAG_RXNE) == RESET)
		;

	// Return received byte
	return (uint8_t) SPI_I2S_ReceiveData(nRF24_SPI_PORT);
}

///**
// * Transfer a byte over SPI1  A4/SS, A5/SCK, A6/MISO, A7/MOSI.
// */
//uint8_t SendReceiveByte_SPI1_Master(uint8_t outByte) {
//
//	// Approach 1, from Brown's book
//	// SPI_I2S_SendData(SPI1, outByte); // send
//	// while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
//	// return SPI_I2S_ReceiveData(SPI1); // read received
//
//	// Approach 2,
//	// from http://www.lxtronic.com/index.php/basic-spi-simple-read-write
//	while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE))
//		;
//	SPI_I2S_SendData(SPI1, outByte); // send
//	while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE))
//		;
//	return SPI_I2S_ReceiveData(SPI1); // read received
//}

