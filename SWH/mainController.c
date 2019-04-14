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
#include "ds1307_mid_level.h"
#include "nrf24_mid_level.h"
#include "debugUsart.h"
#include "eeprom.h"

#define BOILER_PORT			GPIOA
#define BOILER_GPIO			GPIO_Pin_9

#define COLLECTOR_PORT		GPIOA
#define COLLECTOR_GPIO		GPIO_Pin_10

uint8_t m_boilerPump;
uint8_t m_collectorPump;

uint8_t m_currentIndex;
uint8_t m_deltaPlus;
uint8_t m_deltaMinus;

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

void TurnOnBoilerPump() {
	m_boilerPump = true;
	Set_Boiler_Pump(true);
	debug.printf("Pumpa na kotlu ukljucena\r\n");
}

void TurnOffBoilerPump() {
	m_boilerPump = false;
	Set_Boiler_Pump(false);
	debug.printf("Pumpa na kotlu iskljucena\r\n");
}

void TurnOnCollectorPump() {
	m_collectorPump = true;
	Set_Collector_Pump(true);
	debug.printf("Pumpa na kolektorima ukljucena\r\n");
}

void TurnOffCollectorPump() {
	m_collectorPump = false;
	Set_Collector_Pump(false);
	debug.printf("Pumpa na kolektorima iskljucena\r\n");
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
	if (m_temperature[T_BOILER] >= (m_temperature[T_WATER_HEATER] + m_deltaPlus)
			&& !m_boilerPump) {
		TurnOnBoilerPump();
	} else if (m_temperature[T_BOILER]
			<= (m_temperature[T_WATER_HEATER] + m_deltaMinus) && m_boilerPump) {
		TurnOffBoilerPump();
	}

	if (m_tCollector.i >= (m_temperature[T_WATER_HEATER] + m_deltaPlus)
			&& !m_collectorPump) {
		TurnOnCollectorPump();
	} else if (m_tCollector.i <= (m_temperature[T_WATER_HEATER] + m_deltaMinus)
			&& m_collectorPump) {
		TurnOffCollectorPump();
	}
}

void PrintHistoryData(historyData_t data) {
	PrintTime(&data.time);
	debug.printf(" p_kot:%d, p_kol:%d", data.boilerPump, data.collectorPump);
	debug.printf(" t_kot:%dC, t_boj:%dC, t_kol:%dC", data.tempBoiler,
			data.tempWaterHeater, data.tempCollector);
	debug.printf("\r\n");
}

void PrintHistory(void) {
	uint8_t i;
	for (i = m_currentIndex; i < MAX_NUMBER_OF_HISTORIES; i++) {
		debug.printf("%d. ", i);
		PrintHistoryData(m_EEPROM_Array.Payload.Item.data[i]);
	}
	for (uint8_t i = 0; i < m_currentIndex; i++) {
		debug.printf("%d. ", i);
		PrintHistoryData(m_EEPROM_Array.Payload.Item.data[i]);
	}
}

void LoadParameters(void) {
	if (at24c_read()) {
		m_currentIndex = m_EEPROM_Array.Payload.Item.currentIndex;
		m_deltaPlus = m_EEPROM_Array.Payload.Item.deltaPlus;
		m_deltaMinus = m_EEPROM_Array.Payload.Item.deltaMinus;
	} else {
		m_currentIndex = 0;
		m_deltaPlus = 3;
		m_deltaMinus = 1;
	}
	PrintHistory();
}

