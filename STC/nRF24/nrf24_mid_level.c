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
#include "nrf24.h"
#include "nrf24_low_level.h"
#include "ds1820.h"
#include "debugUsart.h"
#include "systemTicks.h"

/* Private variables ---------------------------------------------------------*/
nrf24_t NRF24Ctx = {
		.RXAddress = { 0xE7, 0xE7, 0xE7, 0xE7, 0xE7 },
		.TXAddress = { 0xD7, 0xD7, 0xD7, 0xD7, 0xD7 },
		.PayloadSize = 2,
		.Channel = 2
};

/* init the hardware pins */
nrf24_error_t nRF24_Init(void) {
	nRF24_CE_H();
	_DelayMS(500);
	nRF24_CE_L();
	nRF24_CSN_H();

	return NRF_SUCCESS;
}

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

void nRF24_Transmit(void) {
	/* Prepare data for sending */
	memset(NRF24Ctx.TXData, 0, sizeof(NRF24Ctx.TXData));
	for (uint8_t i = 0; i < NRF24Ctx.PayloadSize; i++) {
		NRF24Ctx.TXData[i] = i;
	}
	/* Automatically goes to TX mode */
	NRF24Send(&NRF24Ctx);

	/* Wait for transmission to end */
	while (NRF24IsSending())
		;

	/* Make analysis on last tranmission attempt */
	uint8_t sendStatus = NRF24LastMessageStatus();

	if (sendStatus == NRF24_TRANSMISSON_OK) {
		debug.printf("Transmition OK!\r\n");
		ToggleLedInd();
	} else if (sendStatus == NRF24_MESSAGE_LOST) {
		debug.printf("Transmition ERROR!\r\n");
	}
	/* Retranmission count indicates the tranmission quality */
	sendStatus = NRF24RetransmissionCount();
	debug.printf("Retransmition count: %u!\r\n", sendStatus);
}

