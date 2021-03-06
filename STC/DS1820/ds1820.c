/**
 ******************************************************************************
 * @file    ds1820.c
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    21-August-2018
 * @brief   Library for DS1820 Digital Thermometer.
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include "stm32f10x.h"
#include "ds1820.h"
#include "oneWire.h"
#include "systemTicks.h"
#include "debugUsart.h"
#include "defines.h"

u_twoBytes m_temperature[1];

/**
 * @brief  Initializes the one-wire communication for DS1820
 * @retval None
 */
void DS1820_Init(void) {
	GPIO_InitTypeDef GPIO_OneWireInitStruct;
	GPIO_OneWireInitStruct.GPIO_Pin = DS1820_PIN_C;
	GPIO_OneWireInitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_OneWireInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS1820_PORT_C, &GPIO_OneWireInitStruct);
}

/**
 * @brief  Read current temperature
 * @param tempSensor temperature sensor for measuring
 * @retval Measured temperature
 */
int16_t OW_ReadTemperature(TempSensor_t tempSensor) {
	GPIO_TypeDef* ow_port;
	uint16_t ow_pin;

	union {
		int16_t word;

		struct {
			uint8_t lsByte;
			uint8_t msByte;
		};
	} temperature;

	switch (tempSensor) {
	case T_COLLECTOR:
	default:
		ow_port = DS1820_PORT_C;
		ow_pin = DS1820_PIN_C;
		break;
	}

	//    uint8_t power;
	//    power = OneWire_readPower();
	//    OneWire_reset();
	//    OneWire_writeByte(SKIP_ROM);
	//    OneWire_writeByte(CONVERT_TEMPERATURE);
	//    if (power == 0x01) {
	//        // Vdd power: poll for conversion complete
	//        // bit will be 1 when conversion is complete
	//        while (OneWire_readBit() == 0);
	//    } else {
	//        // parasitic power: drive data high and wait conversion time
	//        OW_LAT = 1; //Drive high
	//        OW_TRIS = OUTPUT; //Set as output
	//        __delay_ms(800); // wait 800 ms for conversion to complete
	//    }
	//    OneWire_reset();
	//    OneWire_writeByte(SKIP_ROM);
	//    OneWire_writeByte(READ_SCRATCHPAD);
	//    temperature.lsByte = OneWire_readByte();
	//    temperature.msByte = OneWire_readByte();
	//    // stop data transfer
	//    OneWire_reset();

	OW_reset(ow_port, ow_pin);
	_DelayUS(375);
	OW_writeByte(ow_port, ow_pin, OW_CMD_SKIPROM);
	_DelayUS(12);
	OW_writeByte(ow_port, ow_pin, OW_CONVERT_TEMPERATURE);
	_DelayMS(300);
	OW_reset(ow_port, ow_pin);
	_DelayUS(375);
	OW_writeByte(ow_port, ow_pin, OW_CMD_SKIPROM);
	_DelayUS(12);
	OW_writeByte(ow_port, ow_pin, OW_READ_SCRATCHPAD);
	_DelayUS(12);
	temperature.lsByte = OW_readByte(ow_port, ow_pin);
	_DelayUS(2);
	temperature.msByte = OW_readByte(ow_port, ow_pin);
	OW_reset(ow_port, ow_pin);

	return (int16_t)((float) temperature.word / TEMP_RES);
}

/**
 * @brief  Determine device power source
 * @param tempSensor temperature sensor for measuring
 * @retval Zero returned if parasitic mode otherwise Vdd source
 */
uint8_t OW_ReadPower(TempSensor_t tempSensor) {
	GPIO_TypeDef* ow_port;
	uint16_t ow_pin;

	switch (tempSensor) {
	case T_COLLECTOR:
	default:
		ow_port = DS1820_PORT_C;
		ow_pin = DS1820_PIN_C;
		break;
	}

	OW_reset(ow_port, ow_pin);
	OW_writeByte(ow_port, ow_pin, OW_CMD_SKIPROM);
	OW_writeByte(ow_port, ow_pin, OW_READ_POWERSUPPLY);
	return OW_readByte(ow_port, ow_pin);
}

void MeasureTemperature(TempSensor_t tempSensor) {
	m_temperature[tempSensor].i = OW_ReadTemperature(tempSensor);
}

void MeasureTemperatures(void) {
	MeasureTemperature(T_COLLECTOR);
}

uint8_t *GetCurrentTemperature(TempSensor_t tempSensor) {
	return m_temperature[tempSensor].b;
}

void Debug_PrintTemperatures(void) {
	DebugChangeColorToWHITE();
	debug.printf("Kolektor: %d\r\n", m_temperature[T_COLLECTOR].i);
	DebugChangeColorToGREEN();
}

