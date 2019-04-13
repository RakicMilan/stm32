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

#include "stm32f10x.h"
#include "mainController.h"
#include "ds1820.h"
#include "nrf24_mid_level.h"
#include "debugUsart.h"

uint8_t m_boilerPump = false;
uint8_t m_collectorPump = false;

void WaterPumpController(void) {
	if (m_temperature[T_BOILER] >= (m_temperature[T_WATER_HEATER] + 3)
			&& !m_boilerPump) {
		m_boilerPump = true; // Pumpa na kotlu ukljucena
		debug.printf("Pumpa na kotlu ukljucena\r\n");
	} else if (m_temperature[T_BOILER] <= (m_temperature[T_WATER_HEATER] + 1)
			&& m_boilerPump) {
		m_boilerPump = false; // Pumpa na kotlu iskljucena
		debug.printf("Pumpa na kotlu iskljucena\r\n");
	}

	if (m_tCollector.i >= (m_temperature[T_WATER_HEATER] + 3)
			&& !m_collectorPump) {
		m_collectorPump = true; // Pumpa na kolektorima ukljucena
		debug.printf("Pumpa na kolektorima ukljucena\r\n");
	} else if (m_tCollector.i <= (m_temperature[T_WATER_HEATER] + 1)
			&& m_collectorPump) {
		m_collectorPump = false; // Pumpa na kolektorima iskljucena
		debug.printf("Pumpa na kolektorima iskljucena\r\n");
	}
}

