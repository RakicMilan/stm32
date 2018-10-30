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

/** Defines ----------------------------------------------------------------- */
#define DS1307_I2Cx				I2C2

/** Public function prototypes ---------------------------------------------- */
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

