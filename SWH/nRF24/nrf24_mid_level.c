/**
 ******************************************************************************
 * @file    nrf24_mid_level.c
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    20-September-2018
 * @brief   nRF24 library.
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#include <string.h>
#include "nrf24_mid_level.h"
#include "nrf24_low_level.h"
#include "nrf24.h"
#include "ds1820.h"
#include "debugUsart.h"

/* Private variables ---------------------------------------------------------*/
nrf24_t NRF24Ctx = {
		.RXAddress = { 0xD7, 0xD7, 0xD7, 0xD7, 0xD7 },
		.TXAddress = { 0xE7, 0xE7, 0xE7, 0xE7, 0xE7 },
		.PayloadSize = 2,
		.Channel = 2
};

u_twoBytes m_tCollector;

void nRF24_Initialize(void) {
	nRF24_GPIO_Init();
#ifdef USE_SPI1
	Init_SPI1_Master();
#else
	Init_SPI2_Master();
#endif

	nRF24_Init();

	debug.printf("nRF24L01+ check: ");
	if (nRF24_Check()) {
		debug.printf("OK\r\n");
	} else {
		debug.printf("FAIL\r\n");
	}

	nRF24_Config(&NRF24Ctx);

	NRF24SetRxAddress(&NRF24Ctx);
	NRF24SetTxAddress(&NRF24Ctx);
}

void nRF24_Receive(void) {
	if (NRF24DataReady()) {
		ToggleLedInd();
		NRF24GetData(&NRF24Ctx);
		debug.printf("Received data:\r\n");

		m_tCollector.b[0] = NRF24Ctx.RXData[0];
		m_tCollector.b[1] = NRF24Ctx.RXData[1];

		for (uint8_t i = 0; i < NRF24Ctx.PayloadSize; i++) {
			debug.printf("%2X ", NRF24Ctx.RXData[i]);
		}
		debug.printf("\r\n");
	}
}

