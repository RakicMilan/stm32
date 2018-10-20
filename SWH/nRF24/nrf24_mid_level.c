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

// Define what part of demo will be compiled:
//   0 : disable
//   1 : enable
#define DEMO_RX_SINGLE      1 // Single address receiver (1 pipe)
#define DEMO_RX_SINGLE_ESB  0 // Single address receiver with Enhanced ShockBurst (1 pipe)
// Kinda foolproof :)
#if ((DEMO_RX_SINGLE + DEMO_RX_SINGLE_ESB) != 1)
#error "Define only one DEMO_xx, use the '1' value"
#endif

u_twoBytes m_tCollector;

void nRF24_InitializeRX(void) {
#if (DEMO_RX_SINGLE)
	// This is simple receiver with one RX pipe:
	//   - pipe#1 address: '0xE7 0x1C 0xE3'
	//   - payload: 5 bytes
	//   - RF channel: 115 (2515MHz)
	//   - data rate: 1Mbps
	//   - CRC scheme: 2 byte

	// The transmitter sends a 5-byte packets to the address '0xE7 0x1C 0xE3' without Auto-ACK (ShockBurst disabled)

	// Disable ShockBurst for all RX pipes
	nRF24_DisableAA(0xFF);

	// Set RF channel
	nRF24_SetRFChannel(2);

	// Set data rate
	nRF24_SetDataRate(nRF24_DR_1Mbps);

	// Set CRC scheme
	nRF24_SetCRCScheme(nRF24_CRC_2byte);

	// Set address width, its common for all pipes (RX and TX)
	nRF24_SetAddrWidth(5);

	// Configure RX PIPE#1
	static const uint8_t nRF24_ADDR[] = {0xC0, 0xE7, 0xE7, 0xE7, 0xE7};
	nRF24_SetAddr(nRF24_PIPE0, nRF24_ADDR); // program address for RX pipe #1
	nRF24_SetRXPipe(nRF24_PIPE0, nRF24_AA_OFF, 5);// Auto-ACK: disabled, payload length: 5 bytes

	nRF24_SetRXPipe(nRF24_PIPE2, nRF24_AA_OFF, 5);// Auto-ACK: disabled, payload length: 5 bytes

	// Set operational mode (PRX == receiver)
	nRF24_SetOperationalMode(nRF24_MODE_RX);

	// Wake the transceiver
	nRF24_SetPowerMode(nRF24_PWR_UP);

	// Put the transceiver to the RX mode
	nRF24_CE_H();
#endif // DEMO_RX_SINGLE
#if (DEMO_RX_SINGLE_ESB)
	// This is simple receiver with Enhanced ShockBurst:
	//   - RX address: 'ESB'
	//   - payload: 10 bytes
	//   - RF channel: 115 (2515MHz)
	//   - data rate: 250kbps (minimum possible, to increase reception reliability)
	//   - CRC scheme: 2 byte

	// The transmitter sends a 10-byte packets to the address 'ESB' with Auto-ACK (ShockBurst enabled)

	// Set RF channel
	nRF24_SetRFChannel(115);

	// Set data rate
	nRF24_SetDataRate(nRF24_DR_250kbps);

	// Set CRC scheme
	nRF24_SetCRCScheme(nRF24_CRC_2byte);

	// Set address width, its common for all pipes (RX and TX)
	nRF24_SetAddrWidth(3);

	// Configure RX PIPE
	static const uint8_t nRF24_ADDR[] = { 'E', 'S', 'B' };
	nRF24_SetAddr(nRF24_PIPE1, nRF24_ADDR); // program address for pipe
	nRF24_SetRXPipe(nRF24_PIPE1, nRF24_AA_ON, 10); // Auto-ACK: enabled, payload length: 10 bytes

	// Set TX power for Auto-ACK (maximum, to ensure that transmitter will hear ACK reply)
	nRF24_SetTXPower(nRF24_TXPWR_0dBm);

	// Set operational mode (PRX == receiver)
	nRF24_SetOperationalMode(nRF24_MODE_RX);

	// Clear any pending IRQ flags
	nRF24_ClearIRQFlags();

	// Wake the transceiver
	nRF24_SetPowerMode(nRF24_PWR_UP);

	// Put the transceiver to the RX mode
	nRF24_CE_H();
#endif // DEMO_RX_SINGLE_ESB
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

	// ---TEST--- //
	nRF24_DumpConfig();
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
		// Length of received payload
		uint8_t payload_length;
		// Pipe number
		nRF24_RXResult pipe;

		// Get a payload from the transceiver
		pipe = nRF24_ReadPayload(nRF24_payload, &payload_length);

		m_tCollector.b[0] = nRF24_payload[0];
		m_tCollector.b[1] = nRF24_payload[1];

		// Clear all pending IRQ flags
		nRF24_ClearIRQFlags();

		// Print a payload contents to UART
		debug.printf("RCV PIPE#%d PAYLOAD:>", pipe);
		UART_SendBufHex((char *) nRF24_payload, payload_length);
		debug.printf("<\r\n");
	}
}

