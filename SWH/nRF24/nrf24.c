// Functions to manage the nRF24L01+ transceiver

#include "nrf24.h"
#include "defines.h"
#include "debugUsart.h"

uint8_t NRF24PayloadSize;

/* configure the module */
void nRF24_Config(nrf24_t* ctxNRF24) {
	/* Use static payload length ... */
	NRF24PayloadSize = ctxNRF24->PayloadSize;

	// Set RF channel
	NRF24ConfigRegister(RF_CH, ctxNRF24->Channel);

	// Set length of incoming payload
	NRF24ConfigRegister(RX_PW_P0, 0x00); // Auto-ACK pipe ...
	NRF24ConfigRegister(RX_PW_P1, ctxNRF24->PayloadSize); // Data payload pipe
	NRF24ConfigRegister(RX_PW_P2, 0x00); // Pipe not used
	NRF24ConfigRegister(RX_PW_P3, 0x00); // Pipe not used
	NRF24ConfigRegister(RX_PW_P4, 0x00); // Pipe not used
	NRF24ConfigRegister(RX_PW_P5, 0x00); // Pipe not used

	// 1 Mbps, TX gain: 0dbm
	NRF24ConfigRegister(RF_SETUP, (0 << RF_DR) | ((0x03) << RF_PWR));

	// CRC enable, 1 byte CRC length
	NRF24ConfigRegister(CONFIG, NRF24_CONFIG);

	// Auto Acknowledgment
	NRF24ConfigRegister(EN_AA,
			(1 << ENAA_P0) | (1 << ENAA_P1) | (0 << ENAA_P2) | (0 << ENAA_P3) | (0 << ENAA_P4)
					| (0 << ENAA_P5));

	// Enable RX addresses
	NRF24ConfigRegister(EN_RXADDR,
			(1 << ERX_P0) | (1 << ERX_P1) | (0 << ERX_P2) | (0 << ERX_P3) | (0 << ERX_P4)
					| (0 << ERX_P5));

	// Auto retransmit delay: 1000 us and Up to 15 retransmit trials
	NRF24ConfigRegister(SETUP_RETR, (0x04 << ARD) | (0x0F << ARC));

	// Dynamic length configurations: No dynamic length
	NRF24ConfigRegister(DYNPD,
			(0 << DPL_P0) | (0 << DPL_P1) | (0 << DPL_P2) | (0 << DPL_P3) | (0 << DPL_P4)
					| (0 << DPL_P5));

	// Start listening
	NRF24PowerUpRx();
}

// Check if the nRF24L01 present
// return:
//   1 - nRF24L01 is online and responding
//   0 - received sequence differs from original
uint8_t nRF24_Check(void) {
	uint8_t rxbuf[5];
	uint8_t i;
	uint8_t *ptr = (uint8_t *) nRF24_TEST_ADDR;

	// Write test TX address and read TX_ADDR register
	NRF24WriteRegister(W_REGISTER | TX_ADDR, ptr, 5);
	NRF24ReadRegister(R_REGISTER | TX_ADDR, rxbuf, 5);

	// Compare buffers, return error on first mismatch
	for (i = 0; i < 5; i++) {
		if (rxbuf[i] != *ptr++)
			return 0;
	}

	return 1;
}

/* Set the RX address */
void NRF24SetRxAddress(nrf24_t* ctxNRF24) {
	nRF24_CE_L();
	NRF24WriteRegister(RX_ADDR_P1, ctxNRF24->RXAddress, NRF24_ADDR_LEN);
	nRF24_CE_H();
}

/* Returns the payload length */
uint8_t NRF24TxPayloadLength(void) {
	return NRF24PayloadSize;
}

/* Set the TX address */
void NRF24SetTxAddress(nrf24_t* ctxNRF24) {
	/* RX_ADDR_P0 must be set to the sending addr for auto ack to work. */
	NRF24WriteRegister(RX_ADDR_P0, ctxNRF24->TXAddress, NRF24_ADDR_LEN);
	NRF24WriteRegister(TX_ADDR, ctxNRF24->TXAddress, NRF24_ADDR_LEN);
}

/* Checks if data is available for reading */
/* Returns 1 if data is ready ... */
uint8_t NRF24DataReady(void) {
	// See note in getData() function - just checking RX_DR isn't good enough
	uint8_t status = NRF24GetStatus();

	// We can short circuit on RX_DR, but if it's not set, we still need
	// to check the FIFO for any pending packets
	if (status & (1 << RX_DR)) {
		return true;
	}

	return !NRF24RxFifoEmpty();
}

/* Checks if receive FIFO is empty or not */
uint8_t NRF24RxFifoEmpty(void) {
	uint8_t fifoStatus;

	NRF24ReadRegister(FIFO_STATUS, &fifoStatus, 1);

	return (fifoStatus & (1 << RX_EMPTY) ? true : false);
}

/* Returns the length of data waiting in the RX fifo */
uint8_t NRF24RxPayloadLength(void) {
	uint8_t status;
	nRF24_CSN_L();
	spi_transfer(R_RX_PL_WID);
	status = spi_transfer(0x00);
	nRF24_CSN_H();
	return status;
}

/* Reads payload bytes into data array */
void NRF24GetData(nrf24_t* ctxNRF24) {
	/* Pull down chip select */
	nRF24_CSN_L();

	/* Send cmd to read rx payload */
	spi_transfer(R_RX_PAYLOAD);

	/* Read payload */
	NRF24ReceiveBuffer(ctxNRF24->RXData, ctxNRF24->PayloadSize);

	/* Pull up chip select */
	nRF24_CSN_H();

	/* Reset status register */
	NRF24ConfigRegister(STATUS, (1 << RX_DR));
}

/* Returns the number of retransmissions occured for the last message */
uint8_t NRF24RetransmissionCount(void) {
	uint8_t rv;
	NRF24ReadRegister(OBSERVE_TX, &rv, 1);
	rv = rv & 0x0F;
	return rv;
}

// Sends a data package to the default address. Be sure to send the correct
// amount of bytes as configured as payload on the receiver.
void NRF24Send(nrf24_t* ctxNRF24) {
	/* Go to Standby-I first */
	nRF24_CE_L();

	/* Set to transmitter mode , Power up if needed */
	NRF24PowerUpTx();

	/* Do we really need to flush TX fifo each time ? */
#if 1
	/* Pull down chip select */
	nRF24_CSN_L();

	/* Write cmd to flush transmit FIFO */
	spi_transfer(FLUSH_TX);

	/* Pull up chip select */
	nRF24_CSN_H();
#endif

	/* Pull down chip select */
	nRF24_CSN_L();

	/* Write cmd to write payload */
	spi_transfer(W_TX_PAYLOAD);

	/* Write payload */
	NRF24TransmitBuffer(ctxNRF24->TXData, ctxNRF24->PayloadSize);

	/* Pull up chip select */
	nRF24_CSN_H();

	/* Start the transmission */
	nRF24_CE_H();
}

uint8_t NRF24IsSending(void) {
	uint8_t status;

	/* read the current status */
	status = NRF24GetStatus();

	/* if sending successful (TX_DS) or max retries exceded (MAX_RT). */
	if ((status & ((1 << TX_DS) | (1 << MAX_RT)))) {
		return false; /* false */
	}

	return true; /* true */
}

uint8_t NRF24GetStatus(void) {
	uint8_t rv;
	nRF24_CSN_L();
	rv = spi_transfer(NOP);
	nRF24_CSN_H();
	return rv;
}

uint8_t NRF24LastMessageStatus(void) {
	uint8_t rv;

	rv = NRF24GetStatus();

	/* Transmission went OK */
	if ((rv & ((1 << TX_DS)))) {
		return NRF24_TRANSMISSON_OK;
	}
	/* Maximum retransmission count is reached */
	/* Last message probably went missing ... */
	else if ((rv & ((1 << MAX_RT)))) {
		return NRF24_MESSAGE_LOST;
	}
	/* Probably still sending ... */
	else {
		return 0xFF;
	}
}

void NRF24PowerUpRx(void) {
	nRF24_CSN_L();
	spi_transfer(FLUSH_RX);
	nRF24_CSN_H();

	NRF24ConfigRegister(STATUS, (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT));

	nRF24_CE_L();
	NRF24ConfigRegister(CONFIG, NRF24_CONFIG | ((1 << PWR_UP) | (1 << PRIM_RX)));
	nRF24_CE_H();
}

void NRF24PowerUpTx(void) {
	NRF24ConfigRegister(STATUS, (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT));
	NRF24ConfigRegister(CONFIG, NRF24_CONFIG | ((1 << PWR_UP) | (0 << PRIM_RX)));
}

void NRF24PowerDown(void) {
	nRF24_CE_L();
	NRF24ConfigRegister(CONFIG, NRF24_CONFIG);
}

/* send and receive multiple bytes over SPI */
void NRF24ReceiveBuffer(uint8_t* DataIn, uint8_t Len) {
	uint8_t i;

	for (i = 0; i < Len; i++) {
		DataIn[i] = spi_transfer(0xff);
	}
}

/* send multiple bytes over SPI */
void NRF24TransmitBuffer(uint8_t* Data, uint8_t Len) {
	uint8_t i;

	for (i = 0; i < Len; i++) {
		spi_transfer(Data[i]);
	}
}

/* Clocks only one byte into the given NRF24 register */
void NRF24ConfigRegister(uint8_t Address, uint8_t Value) {
	nRF24_CSN_L();
	spi_transfer(W_REGISTER | (REGISTER_MASK & Address));
	spi_transfer(Value);
	nRF24_CSN_H();
}

/* Read single register from NRF24 */
void NRF24ReadRegister(uint8_t Address, uint8_t* Data, uint8_t Len) {
	nRF24_CSN_L();
	spi_transfer(R_REGISTER | (REGISTER_MASK & Address));
	NRF24ReceiveBuffer(Data, Len);
	nRF24_CSN_H();
}

/* Write to a single register of nrf24 */
void NRF24WriteRegister(uint8_t Address, uint8_t* Data, uint8_t Len) {
	nRF24_CSN_L();
	spi_transfer(W_REGISTER | (REGISTER_MASK & Address));
	NRF24TransmitBuffer(Data, Len);
	nRF24_CSN_H();
}

// Print nRF24L01+ current configuration (for debug purposes)
void nRF24_DumpConfig(void) {
	uint8_t i, j;
	uint8_t aw;
	uint8_t buf[5];

	// Dump nRF24L01+ configuration
	// CONFIG
	NRF24ReadRegister(CONFIG, &i, 1);
	debug.printf("[0x%02X] 0x%02X MASK:0x%02X CRC:0x%02X PWR:%s MODE:P%s\r\n", CONFIG, i, i >> 4,
			(i & 0x0c) >> 2, (i & 0x02) ? "ON" : "OFF", (i & 0x01) ? "RX" : "TX");
	// EN_AA
	NRF24ReadRegister(EN_AA, &i, 1);
	debug.printf("[0x%02X] 0x%02X ENAA: ", EN_AA, i);
	for (j = 0; j < 6; j++) {
		debug.printf("[P%1u%s]%s", j, (i & (1 << j)) ? "+" : "-", (j == 5) ? "\r\n" : " ");
	}
	// EN_RXADDR
	NRF24ReadRegister(EN_RXADDR, &i, 1);
	debug.printf("[0x%02X] 0x%02X EN_RXADDR: ", EN_RXADDR, i);
	for (j = 0; j < 6; j++) {
		debug.printf("[P%1u%s]%s", j, (i & (1 << j)) ? "+" : "-", (j == 5) ? "\r\n" : " ");
	}
	// SETUP_AW
	NRF24ReadRegister(SETUP_AW, &i, 1);
	aw = (i & 0x03) + 2;
	debug.printf("[0x%02X] 0x%02X EN_RXADDR=0x%02X (address width = %d)\r\n", SETUP_AW, i, i & 0x03,
			aw);
	// SETUP_RETR
	NRF24ReadRegister(SETUP_RETR, &i, 1);
	debug.printf("[0x%02X] 0x%02X ARD=0x%02X ARC=0x%02X (retr.delay=%d[us], count=%d)\r\n",
			SETUP_RETR, i, i >> 4, i & 0x0F, ((i >> 4) * 250) + 250, i & 0x0F);
	// RF_CH
	NRF24ReadRegister(RF_CH, &i, 1);
	debug.printf("[0x%02X] 0x%02X %d[GHz]\r\n", RF_CH, i, 2400 + i);
	// RF_SETUP
	NRF24ReadRegister(RF_SETUP, &i, 1);
	debug.printf("[0x%02X] 0x%02X CONT_WAVE:%s PLL_LOCK:%s DataRate=", RF_SETUP, i,
			(i & 0x80) ? "ON" : "OFF", (i & 0x80) ? "ON" : "OFF");
	switch ((i & 0x28) >> 3) {
	case 0x00:
		debug.printf("1M");
		break;
	case 0x01:
		debug.printf("2M");
		break;
	case 0x04:
		debug.printf("250k");
		break;
	default:
		debug.printf("???");
		break;
	}
	debug.printf("pbs RF_PWR=");
	switch ((i & 0x06) >> 1) {
	case 0x00:
		debug.printf("-18");
		break;
	case 0x01:
		debug.printf("-12");
		break;
	case 0x02:
		debug.printf("-6");
		break;
	case 0x03:
		debug.printf("0");
		break;
	default:
		debug.printf("???");
		break;
	}
	debug.printf("dBm\r\n");
	// STATUS
	NRF24ReadRegister(STATUS, &i, 1);
	debug.printf("[0x%02X] 0x%02X IRQ:0x%02X RX_PIPE:%d TX_FULL:%s\r\n", STATUS, i, (i & 0x70) >> 4,
			(i & 0x0E) >> 1, (i & 0x01) ? "YES" : "NO");
	// OBSERVE_TX
	NRF24ReadRegister(OBSERVE_TX, &i, 1);
	debug.printf("[0x%02X] 0x%02X PLOS_CNT=%d ARC_CNT=%d\r\n", OBSERVE_TX, i, i >> 4, i & 0x0F);
	// RPD
	NRF24ReadRegister(RPD, &i, 1);
	debug.printf("[0x%02X] 0x%02X RPD=%s\r\n", RPD, i, (i & 0x01) ? "YES" : "NO");
	// RX_ADDR_P0
	NRF24ReadRegister(RX_ADDR_P0, buf, aw);
	debug.printf("[0x%02X] RX_ADDR_P0 \"", RX_ADDR_P0);
	for (i = 0; i < aw; i++)
		debug.printf("0x%02X ", buf[i]);
	debug.printf("\"\r\n");
	// RX_ADDR_P1
	NRF24ReadRegister(RX_ADDR_P1, buf, aw);
	debug.printf("[0x%02X] RX_ADDR_P1 \"", RX_ADDR_P1);
	for (i = 0; i < aw; i++)
		debug.printf("0x%02X ", buf[i]);
	debug.printf("\"\r\n");
	// RX_ADDR_P2
	debug.printf("[0x%02X] RX_ADDR_P2 \"", RX_ADDR_P2);
	for (i = 0; i < aw - 1; i++)
		debug.printf("0x%02X ", buf[i]);
	NRF24ReadRegister(RX_ADDR_P2, &i, 1);
	debug.printf("0x%02X \"\r\n", i);
	// RX_ADDR_P3
	debug.printf("[0x%02X] RX_ADDR_P3 \"", RX_ADDR_P3);
	for (i = 0; i < aw - 1; i++)
		debug.printf("0x%02X ", buf[i]);
	NRF24ReadRegister(RX_ADDR_P3, &i, 1);
	debug.printf("0x%02X \"\r\n", i);
	// RX_ADDR_P4
	debug.printf("[0x%02X] RX_ADDR_P4 \"", RX_ADDR_P4);
	for (i = 0; i < aw - 1; i++)
		debug.printf("0x%02X ", buf[i]);
	NRF24ReadRegister(RX_ADDR_P4, &i, 1);
	debug.printf("0x%02X \"\r\n", i);
	// RX_ADDR_P5
	debug.printf("[0x%02X] RX_ADDR_P5 \"", RX_ADDR_P5);
	for (i = 0; i < aw - 1; i++)
		debug.printf("0x%02X ", buf[i]);
	NRF24ReadRegister(RX_ADDR_P5, &i, 1);
	debug.printf("0x%02X \"\r\n", i);
	// TX_ADDR
	NRF24ReadRegister(TX_ADDR, buf, aw);
	debug.printf("[0x%02X] TX_ADDR    \"", TX_ADDR);
	for (i = 0; i < aw; i++)
		debug.printf("0x%02X ", buf[i]);
	debug.printf("\"\r\n");
	// RX_PW_P0
	NRF24ReadRegister(RX_PW_P0, &i, 1);
	debug.printf("[0x%02X] RX_PW_P0=%d\r\n", RX_PW_P0, i);
	// RX_PW_P1
	NRF24ReadRegister(RX_PW_P1, &i, 1);
	debug.printf("[0x%02X] RX_PW_P1=%d\r\n", RX_PW_P1, i);
	// RX_PW_P2
	NRF24ReadRegister(RX_PW_P2, &i, 1);
	debug.printf("[0x%02X] RX_PW_P2=%d\r\n", RX_PW_P2, i);
	// RX_PW_P3
	NRF24ReadRegister(RX_PW_P3, &i, 1);
	debug.printf("[0x%02X] RX_PW_P3=%d\r\n", RX_PW_P3, i);
	// RX_PW_P4
	NRF24ReadRegister(RX_PW_P4, &i, 1);
	debug.printf("[0x%02X] RX_PW_P4=%d\r\n", RX_PW_P4, i);
	// RX_PW_P5
	NRF24ReadRegister(RX_PW_P5, &i, 1);
	debug.printf("[0x%02X] RX_PW_P5=%d\r\n", RX_PW_P5, i);
}

