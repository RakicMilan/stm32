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

///**
// * This funcion initializes the SPI1 peripheral.
// * Setup SPI1 pins A4/SS, A5/SCK, A6/MISO, A7/MOSI on port A.
// * Hardware slave select is not used. That is indicated in the settings.
// */
//void Init_SPI1_Master(void) {
//	GPIO_InitTypeDef GPIO_InitDef;
//	SPI_InitTypeDef SPI_InitDef;
//
//	// initialize init structs
//	GPIO_StructInit(&GPIO_InitDef);
//	SPI_StructInit(&SPI_InitDef);
//
//	// initialize clocks
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA,
//			ENABLE);
//	// do not initialize A4/SS because a software SS will be used
//
//	// initialize A5/SCK alternate function push-pull (50 MHz)
//	GPIO_InitDef.GPIO_Pin = GPIO_Pin_5;
//	GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitDef);
//
//	// initialize A6/MISO input pull-up (50MHz)
//	GPIO_InitDef.GPIO_Pin = GPIO_Pin_6;
//	GPIO_InitDef.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitDef);
//
//	// initialize A7/MOSI alternate function push-pull (50 MHz)
//	GPIO_InitDef.GPIO_Pin = GPIO_Pin_7;
//	GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitDef);
//
//	// initialize SPI master
//	// for slave, no need to define SPI_BaudRatePrescaler
//	SPI_InitDef.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//	SPI_InitDef.SPI_Mode = SPI_Mode_Master;
//	SPI_InitDef.SPI_DataSize = SPI_DataSize_8b; // 8-bit transactions
//	SPI_InitDef.SPI_FirstBit = SPI_FirstBit_MSB; // MSB first
//	SPI_InitDef.SPI_CPOL = SPI_CPOL_Low; // CPOL = 0, clock idle low
//	SPI_InitDef.SPI_CPHA = SPI_CPHA_2Edge; // CPHA = 1
//	SPI_InitDef.SPI_NSS = SPI_NSS_Soft; // use software SS
//	SPI_InitDef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; // APB2 72/64 = 1.125 MHz
//	// SPI_InitDef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; // APB2 72/256 = 0.28 MHz
//	// SPI_InitDef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // APB2 72/16 = 4.5 MHz
//	SPI_InitDef.SPI_CRCPolynomial = 7;
//	SPI_Init(SPI1, &SPI_InitDef);
//
//	SPI_Cmd(SPI1, ENABLE);
//}

/**
 * This funcion initializes the SPI1 peripheral.
 * Setup SPI2 pins B12/SS, B13/SCK, B14/MISO, B15/MOSI on port B
 * Hardware slave select is not used. That is indicated in the settings.
 */
void Init_SPI2_Master(void) {
	GPIO_InitTypeDef PORT;
	SPI_InitTypeDef SPI;

//	// initialize init structs
//	GPIO_StructInit(&PORT);
//	SPI_StructInit(&SPI);

// Enable SPI2 peripheral
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	// Enable SPI2 GPIO peripheral (PORTB)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//	// initialize clocks
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);

// Configure nRF24 IRQ pin
	PORT.GPIO_Mode = GPIO_Mode_Out_PP;
	PORT.GPIO_Speed = GPIO_Speed_2MHz;
	PORT.GPIO_Pin = nRF24_IRQ_PIN;
	GPIO_Init(nRF24_IRQ_PORT, &PORT);

	// Configure SPI pins (SPI2)
	PORT.GPIO_Mode = GPIO_Mode_AF_PP;
	PORT.GPIO_Speed = GPIO_Speed_50MHz;
	PORT.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &PORT);

//	// initialize B13/SCK alternate function push-pull (10 MHz)
//	GPIO_InitDef.GPIO_Pin = GPIO_Pin_13;
//	GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitDef.GPIO_Speed = GPIO_Speed_10MHz;
//	GPIO_Init(GPIOB, &GPIO_InitDef);
//
//	// initialize B14/MISO input pull-up (10 MHz)
//	GPIO_InitDef.GPIO_Pin = GPIO_Pin_14;
//	GPIO_InitDef.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_InitDef.GPIO_Speed = GPIO_Speed_10MHz;
//	GPIO_Init(GPIOB, &GPIO_InitDef);
//
//	// initialize B15/MOSI alternate function push-pull (10 MHz)
//	GPIO_InitDef.GPIO_Pin = GPIO_Pin_15;
//	GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitDef.GPIO_Speed = GPIO_Speed_10MHz;
//	GPIO_Init(GPIOB, &GPIO_InitDef);

// Initialize SPI2
	SPI.SPI_Mode = SPI_Mode_Master;
	SPI.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI.SPI_CPOL = SPI_CPOL_Low;
	SPI.SPI_CPHA = SPI_CPHA_1Edge;
	SPI.SPI_CRCPolynomial = 7;
	SPI.SPI_DataSize = SPI_DataSize_8b;
	SPI.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(nRF24_SPI_PORT, &SPI);
	SPI_NSSInternalSoftwareConfig(nRF24_SPI_PORT, SPI_NSSInternalSoft_Set);
	SPI_Cmd(nRF24_SPI_PORT, ENABLE);

//	//  initialize SPI master
//	// for slave, no need to define SPI_BaudRatePrescaler
//	SPI_InitDef.SPI_CPHA = SPI_CPHA_2Edge; // CPHA = 1
//	SPI_InitDef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; // APB1 36/32 = 1.125 MHz
//	// SPI_InitDef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128; // APB1 36/128 = 0.28 MHz
//	// SPI_InitDef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; // APB1 36/8 = 4.5 MHz
}

///**
// * Setup slave select, output, A3 on port A.
// */
//void Init_SS() {
//	GPIO_InitTypeDef GPIO_InitDef; // GPIO init
//	GPIO_StructInit(&GPIO_InitDef); // initialize init struct
//
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // initialize clock
//
//	GPIO_InitDef.GPIO_Pin = GPIO_Pin_3; // GPIO pin 3
//	GPIO_InitDef.GPIO_Mode = GPIO_Mode_Out_PP; // GPIO mode output push-pull
//	GPIO_InitDef.GPIO_Speed = GPIO_Speed_2MHz; // GPIO port output speed, 2 MHz
//	GPIO_Init(GPIOA, &GPIO_InitDef); // initialize pin on GPIOA port
//
//	GPIO_SetBits(GPIOA, GPIO_Pin_3); // set bit/pin, slave not selected
//}

// Configure the GPIO lines of the nRF24L01 transceiver
// note: IRQ pin must be configured separately
void nRF24_GPIO_Init(void) {
	GPIO_InitTypeDef PORT;

	// Enable the nRF24L01 GPIO peripherals
	RCC->APB2ENR |= nRF24_GPIO_PERIPHERALS;

	// Configure CSN pin
	PORT.GPIO_Mode = GPIO_Mode_Out_PP;
	PORT.GPIO_Speed = GPIO_Speed_2MHz;
	PORT.GPIO_Pin = nRF24_CSN_PIN;
	GPIO_Init(nRF24_CSN_PORT, &PORT);
	nRF24_CSN_H();

	// Configure CE pin
	PORT.GPIO_Pin = nRF24_CE_PIN;
	GPIO_Init(nRF24_CE_PORT, &PORT);
	nRF24_CE_L();
}

/**
 * Transfer a byte over SPI1  A4/SS, A5/SCK, A6/MISO, A7/MOSI.
 */
uint8_t SendReceiveByte_SPI1_Master(uint8_t outByte) {

	// Approach 1, from Brown's book
	// SPI_I2S_SendData(SPI1, outByte); // send
	// while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	// return SPI_I2S_ReceiveData(SPI1); // read received

	// Approach 2,
	// from http://www.lxtronic.com/index.php/basic-spi-simple-read-write
	while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE))
		;
	SPI_I2S_SendData(SPI1, outByte); // send
	while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE))
		;
	return SPI_I2S_ReceiveData(SPI1); // read received
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

//#define CMD_SET_READ_BIT 0x80
//#define REG_WHO_AM_I 0x0F
//#define VAL_WHO_AM_I 0xD7

//void SPI1_Test() {
//	uint8_t numRead = 0;
//
//	GPIO_ResetBits(GPIOA, GPIO_Pin_3); // slave select (low)
//
//	// 1st bit to 1 indicate read 0x80. 2nd bit can be zero, don't care
//	SendReceiveByte_SPI1_Master(CMD_SET_READ_BIT | REG_WHO_AM_I); // address WHO_AM_I
//	numRead = SendReceiveByte_SPI1_Master(0xFF); // value WHO_AM_I
//
//	GPIO_SetBits(GPIOA, GPIO_Pin_3); // slave deselect (high)
//	_DelayMS(500);
//
//	// if correct value is received, then the L3GD20H is detected
//	if (numRead == VAL_WHO_AM_I) {
//		debug.printf("Detected L3GD202H\n");
//	} else {
//		debug.printf("Failed to detect L3GD202H\n");
//	}
//
//	numRead = 0x00;
//}

