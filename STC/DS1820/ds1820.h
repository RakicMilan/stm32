/**
 ******************************************************************************
 * @file    ds1820.h
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    21-August-2018
 * @brief   Library for DS1820 Digital Thermometer.
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#ifndef __DS1820_H
#define __DS1820_H

#define DS1820_PIN_C		GPIO_Pin_11
#define DS1820_PORT_C		GPIOA

/* temperature resolution => 1/256°C = 0.0039°C */
#define TEMP_RES              0x10 //Calculation for DS18B20 with 0.1 deg C resolution
//#define TEMP_RES              0x02 //Calculation for DS18S20 with 0.5 deg C resolution

typedef enum {
    T_COLLECTOR = 0
} TempSensor_t;

void DS1820_Init(void);
int16_t OW_ReadTemperature(TempSensor_t tempSensor);
uint8_t OW_ReadPower(TempSensor_t tempSensor);
void MeasureTemperatures(void);
unsigned char *GetCurrentTemperature(TempSensor_t tempSensor);
void Debug_PrintTemperatures(void);

#endif	/* __DS1820_H */
