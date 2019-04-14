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
#include "ds1307.h"
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

uint8_t TemperatureIsValid(int16_t temperature) {
	if (temperature < -20 || temperature > 100) {
		return false;
	} else {
		return true;
	}
}

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

void PrintHistoryData(historyData_t data) {
	PrintTime(&data.time);
	debug.printf(" p_kot:%d, p_kol:%d", data.boilerPump, data.collectorPump);
	debug.printf(" t_kot:%dC, t_boj:%dC, t_kol:%dC", data.tempBoiler,
			data.tempWaterHeater, data.tempCollector);
	debug.printf("\r\n");
}

void SetAndWriteCurrentData(void) {
	m_EEPROM_Array.Payload.Item.data[m_currentIndex].time.date =
			ds1307_get_date();
	m_EEPROM_Array.Payload.Item.data[m_currentIndex].time.month =
			ds1307_get_month();
	m_EEPROM_Array.Payload.Item.data[m_currentIndex].time.year =
			ds1307_get_year();
	m_EEPROM_Array.Payload.Item.data[m_currentIndex].time.hours_24 =
			ds1307_get_hours_24();
	m_EEPROM_Array.Payload.Item.data[m_currentIndex].time.minutes =
			ds1307_get_minutes();
	m_EEPROM_Array.Payload.Item.data[m_currentIndex].time.seconds =
			ds1307_get_seconds();

	m_EEPROM_Array.Payload.Item.data[m_currentIndex].boilerPump = m_boilerPump;
	m_EEPROM_Array.Payload.Item.data[m_currentIndex].collectorPump =
			m_collectorPump;

	m_EEPROM_Array.Payload.Item.data[m_currentIndex].tempBoiler =
			m_temperature[T_BOILER];
	m_EEPROM_Array.Payload.Item.data[m_currentIndex].tempWaterHeater =
			m_temperature[T_WATER_HEATER];
	m_EEPROM_Array.Payload.Item.data[m_currentIndex].tempCollector =
			m_tCollector.i;

	at24c_write();
	PrintHistoryData(m_EEPROM_Array.Payload.Item.data[m_currentIndex]);
	++m_currentIndex;
}

void SetAndWriteDelta(void) {
	m_EEPROM_Array.Payload.Item.deltaPlus = m_deltaPlus;
	m_EEPROM_Array.Payload.Item.deltaMinus = m_deltaMinus;

	at24c_write();
	debug.printf("DELTA +:%d, DELTA -:%d\r\n", m_deltaPlus, m_deltaMinus);
}

void TurnOnBoilerPump() {
	m_boilerPump = true;
	Set_Boiler_Pump(true);
	SetAndWriteCurrentData();
}

void TurnOffBoilerPump() {
	m_boilerPump = false;
	Set_Boiler_Pump(false);
	SetAndWriteCurrentData();
}

void TurnOnCollectorPump() {
	m_collectorPump = true;
	Set_Collector_Pump(true);
	SetAndWriteCurrentData();
}

void TurnOffCollectorPump() {
	m_collectorPump = false;
	Set_Collector_Pump(false);
	SetAndWriteCurrentData();
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
	if (TemperatureIsValid(m_temperature[T_WATER_HEATER])) {
		if (TemperatureIsValid(m_temperature[T_BOILER])) {
			if (m_temperature[T_BOILER]
					>= (m_temperature[T_WATER_HEATER] + m_deltaPlus)
					&& !m_boilerPump) {
				TurnOnBoilerPump();
			} else if (m_temperature[T_BOILER]
					<= (m_temperature[T_WATER_HEATER] + m_deltaMinus)
					&& m_boilerPump) {
				TurnOffBoilerPump();
			}
		}

		if (TemperatureIsValid(m_tCollector.i)) {
			if (m_tCollector.i >= (m_temperature[T_WATER_HEATER] + m_deltaPlus)
					&& !m_collectorPump) {
				TurnOnCollectorPump();
			} else if (m_tCollector.i
					<= (m_temperature[T_WATER_HEATER] + m_deltaMinus)
					&& m_collectorPump) {
				TurnOffCollectorPump();
			}
		}
	}
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

void IncreaseDeltaPlus(void) {
	if (m_deltaPlus < 10) {
		m_deltaPlus++;
	} else {
		m_deltaPlus = m_deltaMinus + 1;
	}
	SetAndWriteDelta();
}

void DecreaseDeltaPlus(void) {
	if (m_deltaPlus > m_deltaMinus + 1) {
		m_deltaPlus--;
	} else {
		m_deltaPlus = 10;
	}
	SetAndWriteDelta();
}

void IncreaseDeltaMinus(void) {
	if (m_deltaMinus < m_deltaPlus - 1) {
		m_deltaMinus++;
	} else {
		m_deltaMinus = 0;
	}
	SetAndWriteDelta();
}

void DecreaseDeltaMinus(void) {
	if (m_deltaMinus > 0) {
		m_deltaMinus--;
	} else {
		m_deltaMinus = m_deltaPlus - 1;
	}
	SetAndWriteDelta();
}

