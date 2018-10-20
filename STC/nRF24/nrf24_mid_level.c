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

#include "nrf24.h"
#include "nrf24_low_level.h"
#include "ds1820.h"
#include "debugUsart.h"
#include "systemTicks.h"

// Define what part of demo will be compiled:
//   0 : disable
//   1 : enable
#define DEMO_TX_SINGLE      1 // Single address transmitter (1 pipe)
#define DEMO_TX_SINGLE_ESB  0 // Single address transmitter with Enhanced ShockBurst (1 pipe)
// Kinda foolproof :)
#if ((DEMO_TX_SINGLE + DEMO_TX_SINGLE_ESB) != 1)
#error "Define only one DEMO_xx, use the '1' value"
#endif

#if (DEMO_TX_SINGLE)
#define nRF24_PAYLOAD_LEN		5
#endif // DEMO_TX_SINGLE
#if (DEMO_TX_SINGLE_ESB)
#define nRF24_PAYLOAD_LEN		10
#endif // DEMO_TX_SINGLE_ESB
// Timeout counter (depends on the CPU speed)
// Used for not stuck waiting for IRQ
#define nRF24_WAIT_TIMEOUT		(uint32_t)0x000FFFFF

// Result of packet transmission
typedef enum {
	nRF24_TX_ERROR = (uint8_t) 0x00, // Unknown error
	nRF24_TX_SUCCESS, // Packet has been transmitted successfully
	nRF24_TX_TIMEOUT, // It was timeout during packet transmit
	nRF24_TX_MAXRT // Transmit failed with maximum auto retransmit count
} nRF24_TXResult;

/**
 * @brief  Function to transmit data packet.
 * @param  pBuf: pointer to the buffer with data to transmit.
 * @param  length: length of the data buffer in bytes.
 * @retval one of nRF24_TX_xx values
 */
nRF24_TXResult nRF24_TransmitPacket(uint8_t *pBuf, uint8_t length) {
	volatile uint32_t wait = nRF24_WAIT_TIMEOUT;
	uint8_t status;

	// Deassert the CE pin (in case if it still high)
	nRF24_CE_L();

	// Transfer a data from the specified buffer to the TX FIFO
	nRF24_WritePayload(pBuf, length);

	// ---TEST--- //
	// CONFIG
	status = nRF24_GetConfig();
	debug.printf("[0x%02X] 0x%02X MASK:0x%02X CRC:0x%02X PWR:%s MODE:P%s\r\n",
			nRF24_REG_CONFIG, status, status >> 4, (status & 0x0C) >> 2,
			(status & 0x02) ? "ON" : "OFF", (status & 0x01) ? "RX" : "TX");

	// Start a transmission by asserting CE pin (must be held at least 10us)
	nRF24_CE_H();

	_DelayMS(50);

	// Deassert the CE pin (Standby-II --> Standby-I)
	nRF24_CE_L();

	// Poll the transceiver status register until one of the following flags will be set:
	//   TX_DS  - means the packet has been transmitted
	//   MAX_RT - means the maximum number of TX retransmits happened
	// note: this solution is far from perfect, better to use IRQ instead of polling the status
	do {
		status = nRF24_GetStatus();
		debug.printf("[0x%02X] 0x%02X IRQ:0x%02X RX_PIPE:%d TX_FULL:%s\r\n",
				nRF24_REG_STATUS, status, (status & 0x70) >> 4,
				(status & 0x0E) >> 1, (status & 0x01) ? "YES" : "NO");
		if (status & (nRF24_FLAG_TX_DS | nRF24_FLAG_MAX_RT)) {
			break;
		}
	} while (wait--);

	if (!wait) {
		// Timeout
		return nRF24_TX_TIMEOUT;
	}

	// Check the flags in STATUS register
	debug.printf("[%02X]", status);

	// Clear pending IRQ flags
	nRF24_ClearIRQFlags();

	if (status & nRF24_FLAG_MAX_RT) {
		// Auto retransmit counter exceeds the programmed maximum limit (FIFO is not removed)
		return nRF24_TX_MAXRT;
	}

	if (status & nRF24_FLAG_TX_DS) {
		// Successful transmission
		return nRF24_TX_SUCCESS;
	}

	// Some banana happens, a payload remains in the TX FIFO, flush it
	nRF24_FlushTX();

	return nRF24_TX_ERROR;
}

void nRF24_InitializeTX(void) {
#if (DEMO_TX_SINGLE)
	// This is simple transmitter (to one logic address):
	//   - TX address: '0xE7 0x1C 0xE3'
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

	// Configure TX PIPE
	static const uint8_t nRF24_ADDR[] = { 0xC0, 0xE7, 0xE7, 0xE7, 0xE7 };
	nRF24_SetAddr(nRF24_PIPETX, nRF24_ADDR); // program TX address

	// Set TX power (maximum)
	nRF24_SetTXPower(nRF24_TXPWR_0dBm);

	// Set operational mode (PTX == transmitter)
	nRF24_SetOperationalMode(nRF24_MODE_TX);

	// Clear any pending IRQ flags
	nRF24_ClearIRQFlags();

	// Wake the transceiver
	nRF24_SetPowerMode(nRF24_PWR_UP);
#endif // DEMO_TX_SINGLE
#if (DEMO_TX_SINGLE_ESB)
	// This is simple transmitter with Enhanced ShockBurst (to one logic address):
	//   - TX address: 'ESB'
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

	// Configure TX PIPE
	static const uint8_t nRF24_ADDR[] = {'E', 'S', 'B'};
	nRF24_SetAddr(nRF24_PIPETX, nRF24_ADDR); // program TX address
	nRF24_SetAddr(nRF24_PIPE0, nRF24_ADDR);// program address for pipe#0, must be same as TX (for Auto-ACK)

	// Set TX power (maximum)
	nRF24_SetTXPower(nRF24_TXPWR_0dBm);

	// Configure auto retransmit: 10 retransmissions with pause of 2500s in between
	nRF24_SetAutoRetr(nRF24_ARD_2500us, 10);

	// Enable Auto-ACK for pipe#0 (for ACK packets)
	nRF24_EnableAA(nRF24_PIPE0);

	// Set operational mode (PTX == transmitter)
	nRF24_SetOperationalMode(nRF24_MODE_TX);

	// Clear any pending IRQ flags
	nRF24_ClearIRQFlags();

	// Wake the transceiver
	nRF24_SetPowerMode(nRF24_PWR_UP);
#endif // DEMO_TX_SINGLE_ESB
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

	nRF24_InitializeTX();
}

uint32_t j = 0;

#if (DEMO_TX_SINGLE_ESB)
uint32_t packets_lost = 0; // global counter of lost packets
uint8_t otx;
uint8_t otx_plos_cnt;// lost packet count
uint8_t otx_arc_cnt;// retransmit count
#endif // DEMO_TX_SINGLE_ESB
void nRF24_Transmit(void) {
	// Buffer to store a payload of maximum width
	uint8_t nRF24_payload[32];
	uint32_t i;

	// Prepare data packet
	for (i = 0; i < nRF24_PAYLOAD_LEN; i++) {
		nRF24_payload[i] = j++;
		if (j > 0x000000FF)
			j = 0;
	}

	// Print a payload
//	debug.printf("PAYLOAD:>");
//	UART_SendBufHex((char *) nRF24_payload, nRF24_PAYLOAD_LEN);
////	UART_SendBufHex((char *)GetCurrentTemperature(T_COLLECTOR), nRF24_PAYLOAD_LEN);
//	debug.printf("< ... TX: ");

	// Transmit a packet
	nRF24_TXResult tx_res = nRF24_TransmitPacket(nRF24_payload,
			nRF24_PAYLOAD_LEN);
//	nRF24_TXResult tx_res = nRF24_TransmitPacket(GetCurrentTemperature(T_COLLECTOR), nRF24_PAYLOAD_LEN);
#if (DEMO_TX_SINGLE_ESB)
	otx = nRF24_GetRetransmitCounters();
	otx_plos_cnt = (otx & nRF24_MASK_PLOS_CNT) >> 4; // packets lost counter
	otx_arc_cnt = (otx & nRF24_MASK_ARC_CNT);// auto retransmissions counter
#endif // DEMO_TX_SINGLE_ESB
	switch (tx_res) {
	case nRF24_TX_SUCCESS:
		debug.printf("OK");
		break;
	case nRF24_TX_TIMEOUT:
		debug.printf("TIMEOUT");
		break;
	case nRF24_TX_MAXRT:
		debug.printf("MAX RETRANSMIT");
#if (DEMO_TX_SINGLE_ESB)
		packets_lost += otx_plos_cnt;
		nRF24_ResetPLOS();
#endif // DEMO_TX_SINGLE_ESB
		break;
	default:
		debug.printf("ERROR");
		break;
	}
#if (DEMO_TX_SINGLE_ESB)
	debug.printf("   ARC=%d,  LOST=%d\r\n", otx_arc_cnt, packets_lost);
#else
	debug.printf("\r\n");
#endif // DEMO_TX_SINGLE_ESB
}

