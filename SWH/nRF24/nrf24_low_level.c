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

	/*Configure GPIO pin : LED */
	PORT.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC, &PORT);
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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

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
 * This funcion toggles led indication.
 */
void ToggleLedInd(void) {
	/* Toggle LED which connected to PC13*/
	GPIOC->ODR ^= GPIO_Pin_13;
}

/**
 * Low level SPI transmit/receive function (hardware depended)
 * input:
 *   tx - value to transmit via SPI
 * return: value received from SPI
 */
uint8_t spi_transfer(uint8_t tx) {
	uint8_t rx_data = 0;

#ifdef USE_SPI1
	SPI1->DR = tx;
	while (!(SPI1->SR & SPI_SR_TXE)) {
		;
	}
	while (!(SPI1->SR & SPI_SR_RXNE)) {
		;
	}
	while ((SPI1->SR & SPI_SR_BSY)) {
		;
	}
	rx_data = SPI1->DR;
#else
	SPI2->DR = tx;
	while (!(SPI2->SR & SPI_SR_TXE)) {
		;
	}
	while (!(SPI2->SR & SPI_SR_RXNE)) {
		;
	}
	while ((SPI2->SR & SPI_SR_BSY)) {
		;
	}
	rx_data = SPI2->DR;
#endif

	return rx_data;
}

