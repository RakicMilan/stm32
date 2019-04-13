/**
 ******************************************************************************
 * @file    mainController.c
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    13-Aprpil-2019
 * @brief   main controller
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2019 MR</center></h2>
 ******************************************************************************
 */

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>

#include "mainController.h"
#include "ds1820.h"
#include "nrf24_mid_level.h"
#include "debugUsart.h"

#define BOILER_PORT			GPIOA
#define BOILER_GPIO			GPIO_Pin_9

#define COLLECTOR_PORT		GPIOA
#define COLLECTOR_GPIO		GPIO_Pin_10

uint8_t m_boilerPump;
uint8_t m_collectorPump;

void Set_Boiler_Pump(uint8_t aEnabled) {
	if (aEnabled) {
		GPIO_ResetBits(BOILER_PORT, BOILER_GPIO);
	} else {
		GPIO_SetBits(BOILER_PORT, BOILER_GPIO);
	}
}

void Set_Collector_Pump(uint8_t aEnabled) {
	if (aEnabled) {
		GPIO_ResetBits(COLLECTOR_PORT, COLLECTOR_GPIO);
	} else {
		GPIO_SetBits(COLLECTOR_PORT, COLLECTOR_GPIO);
	}
}

void InitWaterPump(void) {
	m_boilerPump = false;
	m_collectorPump = false;

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Pin = BOILER_GPIO;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BOILER_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = COLLECTOR_GPIO;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(COLLECTOR_PORT, &GPIO_InitStruct);

	Set_Boiler_Pump(false);
	Set_Collector_Pump(false);
}

void WaterPumpController(void) {
	if (m_temperature[T_BOILER] >= (m_temperature[T_WATER_HEATER] + 3)
			&& !m_boilerPump) {

		m_boilerPump = true; // Pumpa na kotlu ukljucena
		Set_Boiler_Pump(true);
		debug.printf("Pumpa na kotlu ukljucena\r\n");

	} else if (m_temperature[T_BOILER] <= (m_temperature[T_WATER_HEATER] + 1)
			&& m_boilerPump) {

		m_boilerPump = false; // Pumpa na kotlu iskljucena
		Set_Boiler_Pump(false);
		debug.printf("Pumpa na kotlu iskljucena\r\n");

	}

	if (m_tCollector.i >= (m_temperature[T_WATER_HEATER] + 3)
			&& !m_collectorPump) {

		m_collectorPump = true; // Pumpa na kolektorima ukljucena
		Set_Collector_Pump(true);
		debug.printf("Pumpa na kolektorima ukljucena\r\n");

	} else if (m_tCollector.i <= (m_temperature[T_WATER_HEATER] + 1)
			&& m_collectorPump) {

		m_collectorPump = false; // Pumpa na kolektorima iskljucena
		Set_Collector_Pump(false);
		debug.printf("Pumpa na kolektorima iskljucena\r\n");

	}
}

