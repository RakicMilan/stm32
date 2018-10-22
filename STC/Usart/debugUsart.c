#include <stm32f10x_usart.h>
#include <string.h>
#include <stdio.h>

#include "defines.h"
#include "usart.h"
#include "debugUsart.h"

#define DEBUG_USART		USART2

//variables
volatile Rbuf_st DebugRxBuff;
volatile Tbuf_st DebugTxBuff;
volatile unsigned char m_DebugMsgReceived;

/*
 * NOTE: Enable fputs if we use custom printf function (i.e. tiny_printf or similar)
 *
 * */

/* We need to implement own __FILE struct */
/* FILE struct is used from __FILE */
#if 0
//local functions
int SendChar(int c);

struct __FILE {
	int dummy;
};

/* You need this if you want use printf */
/* Struct FILE is implemented in stdio.h */
FILE __stdout;

int fputs(int ch, FILE *f) {
	/* Do your stuff here */
	/* Send your custom byte */
	/* Send byte to USART */
	SendChar(ch);

	/* If everything is OK, you have to return character written */
	return ch;
	/* If character is not correct, you can return EOF (-1) to stop writing */
	//return -1;
}
#endif

/*------------------------------------------------------------------------------
 buffer_Init
 initialize the buffers
 *------------------------------------------------------------------------------*/
void InitDebugUsart(uint32_t baudrate) {

	memset(&DebugTxBuff, 0, sizeof(DebugTxBuff));
	memset(&DebugRxBuff, 0, sizeof(DebugRxBuff));
	DebugTxBuff.Empty = true;

	m_DebugMsgReceived = false;

	init_USART2(baudrate);

	//init debug log functions
	debug.printf = printf;
}

/*------------------------------------------------------------------------------
 SenChar
 transmit a character
 *------------------------------------------------------------------------------*/
static inline int SendChar(int c) {

	DebugTxBuff.buf[DebugTxBuff.in & (TBUF_SIZE - 1)] = c; // Add data to the transmit buffer.
	DebugTxBuff.in++;
	DebugTxBuff.Empty = false;
	DEBUG_USART->CR1 |= USART_FLAG_TXE; // enable TX interrupt

	return (0);
}

void Send_N_Chars(char *ptr, int len) {
	int i;
	for (i = 0; i < len; i++) {
		// UART_PutChar is user's basic output function
		SendChar((int) *ptr++);
	}
}

void UART_SendBufHex(char *buf, uint16_t bufsize) {
	uint16_t i;
	for (i = 0; i < bufsize; i++) {
		debug.printf("%02X ", *buf++);
	}
}

void PrintAvailableCommands(void) {
	DebugChangeColorToGREEN();
	debug.printf("\r\n? - Print available commands\r\n\r\n");
}

/**
 * Check received byte through USART interface
 * Unused characters: e, u, o, p, j, l, z, m;
 */
void CheckConsoleRx(void) {
	switch (DebugRxBuff.LastChar) {
	//analog values print outs
	case '?':
		PrintAvailableCommands();
		break;
	default:
		debug.printf("[%03d]Wrong Key\r\n", DebugRxBuff.LastChar);
		break;
	}
}

unsigned char IsUsartTxBufferEmpty(void) {
	return DebugTxBuff.Empty;
}

