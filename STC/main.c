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
#include <stm32f10x_pwr.h>
#include <stm32f10x_tim.h>

#include "systemTicks.h"
#include "debugUsart.h"
#include "ds1820.h"
#include "debugMsg.h"
#include "nrf24_mid_level.h"
#include "task.h"

void DefineTasks(void) {
	InitTasks();

	AddTaskSignal(&CheckConsoleRx, &m_DebugMsgReceived, true);

	AddTaskTime(&MeasureTemperatures, TIME(0.5), true);

	AddTaskTime(&PrintTasks, TIME(1), false);
	AddTaskTime(&TaskManager, TIME(1), false);

	AddTaskTime(&nRF24_Transmit, TIME(1), true);
}

void InitPeriphClock(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	/* Enable PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
}

void InitializeTimer(void) {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = 36000;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = 5000;
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4, &timerInitStructure);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}

void EnableTimerInterrupt() {
	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = TIM4_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
	nvicStructure.NVIC_IRQChannelSubPriority = 1;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);
}

void Init(void) {
	DefineTasks();

	InitSystemTicks();
	InitDebugUsart(921600);
	ShowBoardInfo();

	DS1820_Init();

	nRF24_Initialize();

//	InitializeTimer();
//	EnableTimerInterrupt();
}

void EnterToSleepMode(void) {
	// Clear Wake-up flag
	PWR->CR |= PWR_CR_CWUF;

	// Set Sleep on exit bit of Cortex System Control Register
	SCB->SCR |= SCB_SCR_SLEEPONEXIT;

	// Request Wait For Interrupt
	__WFI();
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

//	EnterToSleepMode();
//
//	while (1) {
//		debug.printf("main loop");
//	}
}

void TIM4_IRQHandler() {
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

		MeasureTemperatures();
		nRF24_Transmit();
	}
}

