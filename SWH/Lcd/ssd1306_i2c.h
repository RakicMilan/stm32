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

/** Defines ----------------------------------------------------------------- */
#define SSD1306_I2Cx			I2C1
#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR        (0x3C << 1)
#endif

/** Public function prototypes ---------------------------------------------- */
void ssd1306_Reset(void);
void ssd1306_WriteCommand(uint8_t byte);
void ssd1306_WriteData(uint8_t* buffer, uint16_t buff_size);

#ifdef __cplusplus
}
#endif

#endif	/* __SSD1306_I2C_H */

