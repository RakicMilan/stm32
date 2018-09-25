/**
 ******************************************************************************
 * @file    ds2432.h
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    21-August-2018
 * @brief   DS2432 EEPROM library.
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#ifndef __DS2432_H
#define __DS2432_H

#define DS2432_PIN		GPIO_Pin_0
#define DS2432_PORT		GPIOA

#define PAGE_NUMBERS	4
#define PAGE_SIZE		32

void DS2432_Init(void);
unsigned char DS2432_ReadData(unsigned char *page_data, unsigned char size);
unsigned char DS2432_WriteData(unsigned char *page_data, unsigned char size);

#endif	/* __DS2432_H */
