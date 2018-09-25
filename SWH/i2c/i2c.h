/**
 ******************************************************************************
 * @file		i2c.h
 * @author		Milan Rakic
 * @date		21 September 2018
 ******************************************************************************
 */

#ifndef __I2C_H
#define __I2C_H

#ifdef __cplusplus
extern "C" {
#endif

/** Includes ---------------------------------------------------------------- */
#include "stm32f10x.h"

/** Public function prototypes ---------------------------------------------- */
void i2c_start(I2C_TypeDef* I2Cx);
void i2c_stop(I2C_TypeDef* I2Cx);
void i2c_address_direction(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);
void i2c_transmit(I2C_TypeDef* I2Cx, uint8_t byte);
uint8_t i2c_receive_ack(I2C_TypeDef* I2Cx);
uint8_t i2c_receive_nack(I2C_TypeDef* I2Cx);

#ifdef __cplusplus
}
#endif

#endif

