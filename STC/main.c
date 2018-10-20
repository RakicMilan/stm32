/**
 ******************************************************************************
 * @file    main.c
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    21-August-2018
 * @brief   main C file of Solar Thermal Collector (STC) project
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>

#include "systemTicks.h"
#include "debugUsart.h"
#include "ds1820.h"
#include "task.h"
#include "debugMsg.h"
#include "nrf24_mid_level.h"

void DefineTasks(void) {
	InitTasks();

	AddTaskSignal(&CheckConsoleRx, &m_DebugMsgReceived, true);

	AddTaskTime(&MeasureTemperatures, TIME(0.5), true);
	AddTaskTime(&Debug_PrintTemperatures, TIME(1), true);

	AddTaskTime(&PrintTasks, TIME(1), false);
	AddTaskTime(&TaskManager, TIME(1), false);

	AddTaskTime(&nRF24_Transmit, TIME(5), true);
}

void Init(void) {
	DefineTasks();

	InitSystemTicks();
	InitDebugUsart(921600);
	ShowBoardInfo();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	DS1820_Init();

	nRF24_Initialize();
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

