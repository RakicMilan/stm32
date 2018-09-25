/**
 ******************************************************************************
 * @file    task.c
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    21-August-2018
 * @brief   Library for tasks
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#include <string.h>
#include "task.h"
#include "defines.h"
#include "systemTicks.h"
#include "debugUsart.h"

volatile task_t task[NUMBER_OF_TASKS];
unsigned char currentNumberOfTasks;

void InitTasks(void) {
	memset(&task, 0, sizeof(task_t) * NUMBER_OF_TASKS);
	currentNumberOfTasks = 0;
}

unsigned char AddTaskSignal(pTask_t *aTask, unsigned char *aTrigger, uint8_t aEnable) {
	if (currentNumberOfTasks < NUMBER_OF_TASKS) {
		if (aTask != NULL) {
			task[currentNumberOfTasks].pTask = aTask;
			task[currentNumberOfTasks].pTrigger = aTrigger;
			task[currentNumberOfTasks].useTimer = false;
			task[currentNumberOfTasks].suspend = !aEnable;
			currentNumberOfTasks++;
			return 0;
		} else {
			return -2;
		}
	} else {
		return -1;
	}
}

unsigned char AddTaskTime(pTask_t *aTask, uint32_t aTime, uint8_t aEnable) {
	if (currentNumberOfTasks < NUMBER_OF_TASKS) {
		if (aTask != NULL) {
			task[currentNumberOfTasks].pTask = aTask;
			task[currentNumberOfTasks].TimeOut = aTime;
			task[currentNumberOfTasks].TimeCnt = micros;
			task[currentNumberOfTasks].useTimer = true;

			task[currentNumberOfTasks].pTrigger = NULL;
			task[currentNumberOfTasks].suspend = !aEnable;
			currentNumberOfTasks++;
			return 0;
		} else {
			return -2;
		}
	} else {
		return -1;
	}
}

uint16_t FindTaskPID(pTask_t *aTask) {
	uint16_t i;
	for (i = 0; i < currentNumberOfTasks; i++) {
		if (task[i].pTask == aTask) {
			return i;
		}
	}
	return -1;
}
void SetTaskEnabled(task_t *aTask, unsigned char aEnable) {
	aTask->suspend = !aEnable;
}

void SetTaskEnabled_Func(pTask_t *aFunc, uint8_t aEnable) {
	uint16_t pid;
	pid = FindTaskPID(aFunc);
	SetTaskEnabled(&task[pid], aEnable);
}

void TaskManager(void) {
	unsigned char i;
	for (i = 0; i < currentNumberOfTasks; i++) {
		task[i].Percent = task[i].RunTimeCnt;
		task[i].RunTimeCnt = 0;
	}
}

void PrintTasks(void) {
	unsigned char i;
	DebugChangeColorToWHITE();
	//DebugSetInverse();
	debug.printf("PID   uS\r\n");
	//DebugSetNormal();
	for (i = 0; i < currentNumberOfTasks; i++) {
		debug.printf("[%2d]: %07d\r\n", i, task[i].Percent);
	}
	DebugChangeColorToGREEN();

	DebugMoveCursorUp(i);
}

void DEBUG_SetTaskManagerEnabled(uint8_t aEnable) {
	uint16_t pid;
	pid = FindTaskPID(&TaskManager);
	SetTaskEnabled(&task[pid], aEnable);
	pid = FindTaskPID(&PrintTasks);
	SetTaskEnabled(&task[pid], aEnable);
	if (aEnable == 0) {
		DebugMoveCursorDw(currentNumberOfTasks);
		DebugChangeColorToGREEN();
		debug.printf("^C OK..\r\n");
	}
}

