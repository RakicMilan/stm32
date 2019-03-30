/**
 ******************************************************************************
 * @file    main.c
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    21-August-2018
 * @brief   main C file of Solar water heating (SWH) project
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>

#include "systemTicks.h"
#include "debugUsart.h"
//#include "ds2432.h"
#include "ds1820.h"
#include "task.h"
#include "debugMsg.h"
#include "nrf24_mid_level.h"
#include "i2c.h"
//#include "sw_i2c.h"
#include "ssd1306.h"
//#include "ssd1306_tests.h"
#include "ds1307.h"
//#include "eeprom.h"
#include "ds1307_mid_level.h"

uint8_t m_displayCounter;

void UpdateDisplay(void) {
	if (m_displayCounter < 3) {
		DisplayTime();
	} else {
		DisplayTemperatures();
	}
	m_displayCounter++;
	if (m_displayCounter >= 6)
		m_displayCounter = 0;
}

void DefineTasks(void) {
	InitTasks();

	AddTaskSignal(&CheckConsoleRx, &m_DebugMsgReceived, true);

	AddTaskTime(&MeasureTemperatures, TIME(0.5), true);
	AddTaskTime(&UpdateDisplay, TIME(1), true);

	AddTaskTime(&PrintTasks, TIME(1), false);
	AddTaskTime(&TaskManager, TIME(1), false);

	AddTaskTime(&nRF24_Receive, TIME(0.001), true);
}

void InitPeriphClock(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void Init(void) {
	DefineTasks();

	InitSystemTicks();
	InitDebugUsart(115200);
	ShowBoardInfo();

//	DS2432_Init();
	DS1820_Init();

	nRF24_Initialize();

	init_i2c1_master();
	init_i2c2_master();
//	init_sw_i2c();

//	uint16_t tmpIndex;
//	for (tmpIndex = 0; tmpIndex < MAX_NUMBER_OF_PAYLOAD_BYTES; tmpIndex++) {
//		m_EEPROM_Array.Payload.Byte[tmpIndex] = tmpIndex;
//	}
//	EEPROMPut();
//	EEPROMGet();

	ssd1306_Init();
	_DelayMS(500);
}

int main(void) {

	InitPeriphClock();

	/*
	 * PB3 (used for buzzer-TIM2_CH2) is something from JTAG,
	 * so we must disable it first using remap function...
	 */
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	Init();

	TasksScheduler();
}

