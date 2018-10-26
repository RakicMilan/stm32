/**
 ******************************************************************************
 * @file    nrf24_low_level.h
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    21-August-2018
 * @brief   nRF24 library.
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#ifndef __NRF24_LOW_LEVEL_H
#define __NRF24_LOW_LEVEL_H

// Hardware abstraction layer for NRF24L01+ transceiver (hardware depended functions)
// GPIO pins definition
// GPIO pins initialization and control functions
// SPI transmit functions

// Peripheral libraries
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>

#define USE_SPI1

#ifdef USE_SPI1
#define nRF24_SPI_PORT             SPI1
// CSN (chip select negative)
#define nRF24_CSN_PORT             GPIOA
#define nRF24_CSN_PIN              GPIO_Pin_4
#else
#define nRF24_SPI_PORT             SPI2
// CSN (chip select negative)
#define nRF24_CSN_PORT             GPIOB
#define nRF24_CSN_PIN              GPIO_Pin_12
#endif

// CE (chip enable)
#define nRF24_CE_PORT              GPIOB
#define nRF24_CE_PIN               GPIO_Pin_1

// IRQ pin
#define nRF24_IRQ_PORT             GPIOB
#define nRF24_IRQ_PIN              GPIO_Pin_0

//function prototypes
void nRF24_GPIO_Init(void);
void Init_SPI1_Master(void);
void Init_SPI2_Master(void);

void ToggleLedInd(void);
uint8_t spi_transfer(uint8_t data);

#endif	/* __NRF24_LOW_LEVEL_H */

