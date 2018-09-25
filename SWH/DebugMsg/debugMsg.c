/**
 ******************************************************************************
 * @file    debugMsg.c
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    21-August-2018
 * @brief   Display basic MCU and FW info through USART module.
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#include "debugMsg.h"
#include "stm32f10x.h"
#include "stm32f103_id.h"
#include "system_stm32f10x.h"
#include "debugUsart.h"
#include "defines.h"

void ShowMcuInfo(void) {
	debug.printf("Flash size[kB] : %d\r\n", ID_GetFlashSize());
	debug.printf("MCU unique ID  : 0x%X%X%X%X\r\n", ID_GetUnique16(0), ID_GetUnique16(1),
			ID_GetUnique32(1), ID_GetUnique32(2));
}

void ShowSystemClocks(void) {
	debug.printf("System Freq[Hz]: %d\r\n", SystemCoreClock);
}

void ShowBoardInfo(void) {
	DebugClearScreen();
	DebugChangeColorToGREEN();
	debug.printf("\r\nSWH debug port\r\n");
	debug.printf("Fw Version: %d.%d.%d.%d [%s %s]\r\n", FW_0, FW_1, FW_2, FW_3, __TIME__, __DATE__);
	DebugChangeColorToWHITE();
	ShowMcuInfo();
	ShowSystemClocks();
	DebugChangeColorToGREEN();
}

