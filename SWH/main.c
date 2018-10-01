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
#include "ds2432.h"
#include "ds1820.h"
#include "task.h"
#include "debugMsg.h"
#include "nrf24_mid_level.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"
#include "ds1307.h"

char customChar[8] = { 0b01000, 0b10100, 0b01000, 0b00011, 0b00100, 0b00100,
		0b00100, 0b00011 };

void DefineTasks(void) {
	InitTasks();

	AddTaskSignal(&CheckConsoleRx, &m_DebugMsgReceived, true);

	AddTaskTime(&MeasureTemperatures, TIME(0.5), true);
	AddTaskTime(&DisplayTemperatures, TIME(1), true);
	AddTaskTime(&Debug_PrintTemperatures, TIME(1), true);

	AddTaskTime(&PrintTasks, TIME(1), false);
	AddTaskTime(&TaskManager, TIME(1), false);

//	AddTaskTime(&nRF24_Receive, TIME(0.001), true);
}

void Init(void) {
	DefineTasks();

	InitSystemTicks();
	InitDebugUsart(921600);
	ShowBoardInfo();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	DS2432_Init();
	DS1820_Init();

//	CreateCustomChar(customChar);

//	nRF24_Initialize();

//	ds1307_init();
	ssd1306_Init();
	_DelayMS(1000);
}

int main(void) {
	/*
	 * PB3 (used for buzzer-TIM2_CH2) is something from JTAG,
	 * so we must disable it first using remap function...
	 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	Init();

	TasksScheduler();
}

