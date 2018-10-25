/**
 ******************************************************************************
 * @file		ds1307_i2c.h
 * @author		Milan Rakic
 * @date		21 September 2018
 ******************************************************************************
 */

#ifndef __DS1307_I2C_H
#define __DS1307_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

/** Includes ---------------------------------------------------------------- */
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"

/** Defines ----------------------------------------------------------------- */
#define DS1307_I2Cx_RCC			RCC_APB1Periph_I2C2
#define DS1307_I2Cx				I2C2
#define DS1307_I2C_GPIO			GPIOB
#define DS1307_I2C_PIN_SCL		GPIO_Pin_10
#define DS1307_I2C_PIN_SDA		GPIO_Pin_11

/** Public function prototypes ---------------------------------------------- */
void ds1307_i2c_init(void);

void i2c_write_no_reg(I2C_TypeDef* I2Cx, uint8_t address, uint8_t data);
void i2c_write_with_reg(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t data);
void i2c_write_multi_no_reg(I2C_TypeDef* I2Cx, uint8_t address, uint8_t* data, uint8_t len);
void i2c_write_multi_with_reg(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t* data,
		uint8_t len);
void i2c_read_no_reg(I2C_TypeDef* I2Cx, uint8_t address, uint8_t* data);
void i2c_read_with_reg(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t* data);
void i2c_read_multi_no_reg(I2C_TypeDef* I2Cx, uint8_t address, uint8_t len, uint8_t* data);
void i2c_read_multi_with_reg(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t len,
		uint8_t* data);

#ifdef __cplusplus
}
#endif

#endif	/* __DS1307_I2C_H */

