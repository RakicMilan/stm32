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

#include "nrf24_mid_level.h"
#include "nrf24_low_level.h"
#include "nrf24.h"
#include "debugUsart.h"

u_twoBytes m_tCollector;

// Pipe number
nRF24_RXResult pipe;

// Length of received payload
uint8_t payload_length;

void nRF24_InitializeRX(void) {
	// This is simple receiver with one RX pipe:
	//   - pipe#1 address: '0xE7 0x1C 0xE3'
	//   - payload: 5 bytes
	//   - RF channel: 115 (2515MHz)
	//   - data rate: 250kbps (minimum possible, to increase reception reliability)
	//   - CRC scheme: 2 byte

	// The transmitter sends a 5-byte packets to the address '0xE7 0x1C 0xE3' without Auto-ACK (ShockBurst disabled)

	// Disable ShockBurst for all RX pipes
	nRF24_DisableAA(0xFF);

	// Set RF channel
	nRF24_SetRFChannel(115);

	// Set data rate
	nRF24_SetDataRate(nRF24_DR_250kbps);

	// Set CRC scheme
	nRF24_SetCRCScheme(nRF24_CRC_2byte);

	// Set address width, its common for all pipes (RX and TX)
	nRF24_SetAddrWidth(3);

	// Configure RX PIPE#1
	static const uint8_t nRF24_ADDR[] = { 0xE7, 0x1C, 0xE3 };
	nRF24_SetAddr(nRF24_PIPE1, nRF24_ADDR); // program address for RX pipe #1
	nRF24_SetRXPipe(nRF24_PIPE1, nRF24_AA_OFF, 5); // Auto-ACK: disabled, payload length: 5 bytes

	// Set operational mode (PRX == receiver)
	nRF24_SetOperationalMode(nRF24_MODE_RX);

	// Wake the transceiver
	nRF24_SetPowerMode(nRF24_PWR_UP);

	// Put the transceiver to the RX mode
	nRF24_CE_H();
}

void nRF24_Initialize(void) {
	debug.printf("\r\nSTM32F103C8T6 is online.\r\n");

	Init_SPI2_Master();

	// Initialize the nRF24L01 GPIO pins
	nRF24_GPIO_Init();

	// RX/TX disabled
	nRF24_CE_L();

	// Configure the nRF24L01+
	debug.printf("nRF24L01+ check: ");
	if (!nRF24_Check()) {
		debug.printf("FAIL\r\n");
		while (1)
			;
	}
	debug.printf("OK\r\n");

	// Initialize the nRF24L01 to its default state
	nRF24_Init();

	nRF24_InitializeRX();
}

//
// Constantly poll the status of the RX FIFO and get a payload if FIFO is not empty
//
// This is far from best solution, but it's ok for testing purposes
// More smart way is to use the IRQ pin :)
//
void nRF24_Receive(void) {
	// Buffer to store a payload of maximum width
	uint8_t nRF24_payload[32];

	if (nRF24_GetStatus_RXFIFO() != nRF24_STATUS_RXFIFO_EMPTY) {
		// Get a payload from the transceiver
		pipe = nRF24_ReadPayload(nRF24_payload, &payload_length);

		m_tCollector.b[0] = nRF24_payload[0];
		m_tCollector.b[1] = nRF24_payload[1];

		// Clear all pending IRQ flags
		nRF24_ClearIRQFlags();

		// Print a payload contents to UART
		debug.printf("RCV PIPE#%d", pipe);
		debug.printf(" PAYLOAD:>");
		UART_SendBufHex((char *) nRF24_payload, payload_length);
		debug.printf("<\r\n");
	}
}

