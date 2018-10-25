/**
 ******************************************************************************
 * @file    usart.c
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    21-August-2018
 * @brief   USART library.
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#include <misc.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
//
#include "defines.h"
#include "debugUsart.h"

/* This funcion initializes the USART1 peripheral
 *
 * Arguments: baudrate --> the baudrate at which the USART is
 * 						   supposed to operate
 */
void init_USART1(uint32_t baudrate) {

	/* This is a concept that has to do with the libraries provided by ST
	 * to make development easier the have made up something similar to
	 * classes, called TypeDefs, which actually just define the common
	 * parameters that every peripheral needs to work correctly
	 *
	 * They make our life easier because we don't have to mess around with
	 * the low level stuff of setting bits in the correct registers
	 */

	/* Configure the GPIOs */
	GPIO_InitTypeDef GPIO_InitStructure; // this is for the GPIO pins used as TX and RX
	USART_InitTypeDef USART_InitStruct; // this is for the USART3 initialization
	NVIC_InitTypeDef NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)

	/* enable APB2 peripheral clock for USART1
	 * note that only USART1 and USART6 are connected to APB2
	 * the other USARTs are connected to APB1
	 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* enable the peripheral clock for the pins used by
	 * USART1, PA9 for TX and PA10 for RX
	 */

	/* Configure USART1 Tx (PA.9) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Now the USART_InitStruct is used to define the
	 * properties of USART1
	 */
	USART_InitStruct.USART_BaudRate = baudrate; // the baudrate is set to the value we passed into this init function
	USART_InitStruct.USART_WordLength = USART_WordLength_8b; // we want the data frame size to be 8 bits (standard)
	USART_InitStruct.USART_StopBits = USART_StopBits_1; // we want 1 stop bit (standard)
	USART_InitStruct.USART_Parity = USART_Parity_No; // we don't want a parity bit (standard)
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // we don't want flow control (standard)
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // we want to enable the transmitter and the receiver
	USART_Init(USART1, &USART_InitStruct); // again all the properties are passed to the USART_Init function which takes care of all the bit setting

	/* Here the USART1 receive interrupt is enabled
	 * and the interrupt controller is configured
	 * to jump to the USART1_IRQHandler() function
	 * if the USART1 receive interrupt occurs
	 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // enable the USART3 receive interrupt
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE); // enable the USART3 tx interrupt

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; // we want to configure the USART1 interrupts
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // this sets the priority group of the USART1 interrupts
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // this sets the subpriority inside the group
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // the USART3 interrupts are globally enabled
	NVIC_Init(&NVIC_InitStructure); // the properties are passed to the NVIC_Init function which takes care of the low level stuff

	// finally this enables the complete USART1 peripheral
	USART_Cmd(USART1, ENABLE);
}

/* This funcion initializes the USART2 peripheral
 *
 * Arguments: baudrate --> the baudrate at which the USART is
 * 						   supposed to operate
 */
void init_USART2(uint32_t baudrate) {

	/* This is a concept that has to do with the libraries provided by ST
	 * to make development easier the have made up something similar to
	 * classes, called TypeDefs, which actually just define the common
	 * parameters that every peripheral needs to work correctly
	 *
	 * They make our life easier because we don't have to mess around with
	 * the low level stuff of setting bits in the correct registers
	 */

	/* Configure the GPIOs */
	GPIO_InitTypeDef GPIO_InitStructure; // this is for the GPIO pins used as TX and RX
	USART_InitTypeDef USART_InitStruct; // this is for the USART3 initialization
	NVIC_InitTypeDef NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)

	/* enable APB1 peripheral clock for USART2
	 * note that USART2 is connected to APB1
	 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* enable the peripheral clock for the pins used by
	 * USART1, PA9 for TX and PA10 for RX
	 */

	/* Configure USART1 Tx (PA.2) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART1 Rx (PA.3) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Now the USART_InitStruct is used to define the
	 * properties of USART1
	 */
	USART_InitStruct.USART_BaudRate = baudrate; // the baudrate is set to the value we passed into this init function
	USART_InitStruct.USART_WordLength = USART_WordLength_8b; // we want the data frame size to be 8 bits (standard)
	USART_InitStruct.USART_StopBits = USART_StopBits_1; // we want 1 stop bit (standard)
	USART_InitStruct.USART_Parity = USART_Parity_No; // we don't want a parity bit (standard)
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // we don't want flow control (standard)
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // we want to enable the transmitter and the receiver
	USART_Init(USART2, &USART_InitStruct); // again all the properties are passed to the USART_Init function which takes care of all the bit setting

	/* Here the USART2 receive interrupt is enabled
	 * and the interrupt controller is configured
	 * to jump to the USART2_IRQHandler() function
	 * if the USART2 receive interrupt occurs
	 */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // enable the USART3 receive interrupt
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE); // enable the USART3 tx interrupt

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; // we want to configure the USART1 interrupts
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // this sets the priority group of the USART1 interrupts
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // this sets the subpriority inside the group
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // the USART3 interrupts are globally enabled
	NVIC_Init(&NVIC_InitStructure); // the properties are passed to the NVIC_Init function which takes care of the low level stuff

	// finally this enables the complete USART1 peripheral
	USART_Cmd(USART2, ENABLE);
}

/*----------------------------------------------------------------------------
 USART1_IRQHandler
 Handles USART1 global interrupt request.
 *----------------------------------------------------------------------------*/
void USART1_IRQHandler(void) {
	char c;
	if (USART1->SR & USART_FLAG_RXNE) { // read interrupt
		c = (USART1->DR & 0x1FF);
		DebugRxBuff.LastChar = c;
		m_DebugMsgReceived = true;
//		rbuf.buf[rbuf.in] = (USART1->DR & 0x1FF);
//		if (!m_MsgReceived) {
//			if (c == '\b' && rbuf.in) { /* Back space? */
//				rbuf.in--;
//				SendChar(c);
//			} else if (c == '\r') {
//				SendChar(c);
//				SendChar('\n');
//				rbuf.buf[rbuf.in] = 0; //null termination
//				memcpy(m_RxBuffer, rbuf.buf, rbuf.in); //rbuf.in + 1
//
//				//memset(rbuf.buf, 0, RBUF_SIZE - 1); //clear rec buffer
//				rbuf.in = 0;
//
//				m_MsgReceived = true;
//			} else if (rbuf.in < (RBUF_SIZE - 1)) {
//				rbuf.buf[rbuf.in] = c;
//				SendChar(c);
//				rbuf.in++;
//			}
//		}
		USART1->SR &= ~USART_FLAG_RXNE; // clear interrupt
	}

	if (USART1->SR & USART_FLAG_TXE) {

		if (DebugTxBuff.in != DebugTxBuff.out) {
			USART1->DR = (DebugTxBuff.buf[DebugTxBuff.out & (TBUF_SIZE - 1)]);
			DebugTxBuff.buf[DebugTxBuff.out & (TBUF_SIZE - 1)] = 0x00;
			DebugTxBuff.out++;
		} else {
			USART1->CR1 &= ~USART_FLAG_TXE; // disable TX interrupt if nothing to send
			DebugTxBuff.Empty = true;
		}
		USART1->SR &= ~USART_FLAG_TXE; // clear interrupt
	}
}

/*----------------------------------------------------------------------------
 USART1_IRQHandler
 Handles USART1 global interrupt request.
 *----------------------------------------------------------------------------*/
void USART2_IRQHandler(void) {
	char c;
	if (USART2->SR & USART_FLAG_RXNE) { // read interrupt
		c = (USART2->DR & 0x1FF);
		DebugRxBuff.LastChar = c;
		m_DebugMsgReceived = true;
//		rbuf.buf[rbuf.in] = (USART1->DR & 0x1FF);
//		if (!m_MsgReceived) {
//			if (c == '\b' && rbuf.in) { /* Back space? */
//				rbuf.in--;
//				SendChar(c);
//			} else if (c == '\r') {
//				SendChar(c);
//				SendChar('\n');
//				rbuf.buf[rbuf.in] = 0; //null termination
//				memcpy(m_RxBuffer, rbuf.buf, rbuf.in); //rbuf.in + 1
//
//				//memset(rbuf.buf, 0, RBUF_SIZE - 1); //clear rec buffer
//				rbuf.in = 0;
//
//				m_MsgReceived = true;
//			} else if (rbuf.in < (RBUF_SIZE - 1)) {
//				rbuf.buf[rbuf.in] = c;
//				SendChar(c);
//				rbuf.in++;
//			}
//		}
		USART2->SR &= ~USART_FLAG_RXNE; // clear interrupt
	}

	if (USART2->SR & USART_FLAG_TXE) {

		if (DebugTxBuff.in != DebugTxBuff.out) {
			USART2->DR = (DebugTxBuff.buf[DebugTxBuff.out & (TBUF_SIZE - 1)]);
			DebugTxBuff.buf[DebugTxBuff.out & (TBUF_SIZE - 1)] = 0x00;
			DebugTxBuff.out++;
		} else {
			USART2->CR1 &= ~USART_FLAG_TXE; // disable TX interrupt if nothing to send
			DebugTxBuff.Empty = true;
		}
		USART2->SR &= ~USART_FLAG_TXE; // clear interrupt
	}
}

