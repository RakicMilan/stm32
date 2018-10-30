/**
 ******************************************************************************
 * @file		sw_i2c.h
 * @author		Milan Rakic
 * @date		30 October 2018
 ******************************************************************************
 */

#ifndef __SW_I2C_H
#define __SW_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

// The 24c04 can be treated as two 256 byte eeproms on addresses 50 and 51
#define EEPROM_ADDRESS		0xA0
#define ACKBIT				0x00            // ACK bit
#define NAKBIT				0x80            // NAK bit

#define SCL					GPIO_Pin_6
#define SDA					GPIO_Pin_7
#define I2C_PORT			GPIOB

GPIO_InitTypeDef GPIO_EepromInitStruct;

static inline void Set_SDA_Input(void) {
	GPIO_EepromInitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_EepromInitStruct.GPIO_Pin = SDA;
	GPIO_Init(I2C_PORT, &GPIO_EepromInitStruct);
}

static inline void Set_SDA_Output(void) {
	GPIO_EepromInitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_EepromInitStruct.GPIO_Pin = SDA;
	GPIO_Init(I2C_PORT, &GPIO_EepromInitStruct);
}

static inline void SDA_HIGH(void) {
	Set_SDA_Output();
	I2C_PORT->BSRR = SDA;
}

static inline void SDA_LOW(void) {
	Set_SDA_Output();
	I2C_PORT->BRR = SDA;
}

static inline void SCL_HIGH(void) {
	I2C_PORT->BSRR = SCL;
}

static inline void SCL_LOW(void) {
	I2C_PORT->BRR = SCL;
}

/** Public function prototypes ---------------------------------------------- */
void init_sw_i2c(void);
void bstart(void);
void bstop(void);
unsigned char byte_out(unsigned char data);
unsigned char byte_in(unsigned char ack);

#ifdef __cplusplus
}
#endif

#endif

