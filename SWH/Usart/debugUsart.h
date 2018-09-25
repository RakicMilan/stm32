#ifndef __DEBUGUSART_H
#define __DEBUGUSART_H

#include <stdio.h>

/*----------------------------------------------------------------------------
 Notes:
 The length of the receive and transmit buffers must be a power of 2.
 Each buffer has a next_in and a next_out index.
 If next_in = next_out, the buffer is empty.
 (next_in - next_out) % buffer_size = the number of characters in the buffer.
 *----------------------------------------------------------------------------*/
#define TBUF_SIZE   256	     /*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/
#define RBUF_SIZE   8      /*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/

/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#if TBUF_SIZE < 2
#error TBUF_SIZE is too small.  It must be larger than 1.
#elif ((TBUF_SIZE & (TBUF_SIZE-1)) != 0)
#error TBUF_SIZE must be a power of 2.
#endif

#if RBUF_SIZE < 2
#error RBUF_SIZE is too small.  It must be larger than 1.
#elif ((RBUF_SIZE & (RBUF_SIZE-1)) != 0)
#error RBUF_SIZE must be a power of 2.
#endif

/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
typedef struct {
	unsigned int in; // Next In Index
	unsigned int out; // Next Out Index
	unsigned char LastChar;
	//unsigned char Received;
	char buf[RBUF_SIZE]; // Buffer
} Rbuf_st;

typedef struct {
	unsigned int in; // Next In Index
	unsigned int out; // Next Out Index
	unsigned char Empty;
	char buf[TBUF_SIZE]; // Buffer
} Tbuf_st;

extern volatile Rbuf_st DebugRxBuff;
extern volatile Tbuf_st DebugTxBuff;
extern volatile unsigned char m_DebugMsgReceived;

void InitDebugUsart(uint32_t baudrate);
void UART_SendBufHex(char *buf, uint16_t bufsize);
void CheckConsoleRx(void);
unsigned char IsUsartTxBufferEmpty(void);

extern void USART_puts(volatile char *s);
extern void DebugShowRxBuffer(void);

volatile struct {
	int (*printf)(const char *, ...);
} debug;

extern unsigned char *FloatToString(float argValue);
extern char *BinToString(unsigned char n);

//color macros
#define DebugMoveCursor(x,y){debug.printf("\033[2%d;%df",x,y);}
#define DebugMoveCursorUp(x){debug.printf("\033[%dA",x);}
#define DebugMoveCursorDw(x){printf("\033[%dB",x);}
#define DebugSaveCursorPos(){debug.printf("\033[s");}
#define DebugRecalCursorPos(){debug.printf("\033[u");}

#define DebugSetBold() {debug.printf("\033[1m");}
#define DebugSetNormal() {debug.printf("\033[0m");}
#define DebugSetBlink() {debug.printf("\033[5m");}
#define DebugSetInverse() {debug.printf("\033[7m");}

#define DebugClearScreen() {debug.printf("\033[2J");}
#define DebugChangeColorToGREEN() {debug.printf("\033[0;32m");}
#define DebugChangeColorToRED() {debug.printf("\033[0;31m");}
#define DebugChangeColorToBLUE() {debug.printf("\033[0;34m"); }
#define DebugChangeColorToYELLOW() {debug.printf("\033[0;33m");}
#define DebugChangeColorToCYIAN() {debug.printf("\033[0;36m"); }
#define DebugChangeColorToWHITE() {debug.printf("\033[0;37m"); }
#define DebugChangeColorToPURPLE() {debug.printf("\033[0;35m");}
#define DebugChangeBackColorToBLACK() {debug.printf("\033[40m");}

#endif	/* __DEBUGUSART_H */
