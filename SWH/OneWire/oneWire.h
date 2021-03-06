/**
 ******************************************************************************
 * @file    oneWire.h
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    21-August-2018
 * @brief   1-Wire library.
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#ifndef __ONEWIRE_H
#define __ONEWIRE_H

// Standard Speed
#define DELAY_A			6
#define DELAY_B			64
#define DELAY_C			60
#define DELAY_D			10
#define DELAY_E			9
#define DELAY_F			55
#define DELAY_G			0
#define DELAY_H			480
#define DELAY_I			70
#define DELAY_J			410

/* OneWire commands */
#define OW_CMD_SEARCHROM			0xF0
#define OW_CMD_MATCHROM				0x55
#define OW_CMD_SKIPROM				0xCC

#define OW_CONVERT_TEMPERATURE		0x44
#define OW_READ_SCRATCHPAD			0xBE
#define OW_READ_POWERSUPPLY			0xB4

int OW_reset(GPIO_TypeDef* ow_port, uint16_t ow_pin);
void OW_writeByte(GPIO_TypeDef* ow_port, uint16_t ow_pin, int data);
int OW_readByte(GPIO_TypeDef* ow_port, uint16_t ow_pin);

#endif	/* __ONEWIRE_H */
