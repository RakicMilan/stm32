/**
 ******************************************************************************
 * @file		ssd1306_i2c.h
 * @author		Milan Rakic
 * @date		21 September 2018
 ******************************************************************************
 */

#ifndef __SSD1306_I2C_H
#define __SSD1306_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

/** Includes ---------------------------------------------------------------- */
#include "stm32f10x_rcc.h"

/** Defines ----------------------------------------------------------------- */
#define SSD1306_I2Cx_RCC		RCC_APB1Periph_I2C1
#define SSD1306_I2C_GPIO_RCC	RCC_APB2Periph_GPIOB
#define SSD1306_I2Cx			I2C1
#define SSD1306_I2C_GPIO		GPIOB
#define SSD1306_I2C_PIN_SCL		GPIO_Pin_6
#define SSD1306_I2C_PIN_SDA		GPIO_Pin_7

#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR        (0x3C << 1)
#endif

/** Public function prototypes ---------------------------------------------- */
void ssd1306_i2c_init(void);
void i2c_write_byte(I2C_TypeDef* I2Cx, uint8_t address, uint8_t data);
void i2c_read_byte(I2C_TypeDef* I2Cx, uint8_t address, uint8_t* data);
void i2c_write_data(I2C_TypeDef* I2Cx, uint8_t address, uint8_t* data, uint8_t len);
void i2c_read_data(I2C_TypeDef* I2Cx, uint8_t address, uint8_t len, uint8_t* data);

#ifdef __cplusplus
}
#endif

#endif	/* __SSD1306_I2C_H */

