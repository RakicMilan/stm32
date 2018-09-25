/**
 ******************************************************************************
 * @file    task.h
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    21-August-2018
 * @brief   Library for tasks
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#ifndef __TASK_H
#define __TASK_H

#include <misc.h>
#include "defines.h"
#include "systemTicks.h"

typedef void pTask_t(void);

typedef struct {
	unsigned char *pTrigger;
	unsigned suspend :1;
	uint32_t RunTimeCnt;
	uint32_t TimeOut;
	uint32_t TimeCnt;
	uint32_t Percent;
	unsigned useTimer :1;
	pTask_t *pTask;
} task_t;

#define NUMBER_OF_TASKS			32

extern volatile task_t task[NUMBER_OF_TASKS];
extern unsigned char currentNumberOfTasks;

//function prototypes
void InitTasks(void);
unsigned char AddTaskSignal(pTask_t *aTask, unsigned char *aTrigger, uint8_t aEnable);
unsigned char AddTaskTime(pTask_t *aTask, uint32_t aTime, uint8_t aEnable);
void SetTaskEnabled(task_t *aTask, unsigned char aEnable);
void SetTaskEnabled_Func(pTask_t *aFunc, uint8_t aEnable);

void TaskManager(void);
void PrintTasks(void);
uint16_t FindTaskPID(pTask_t *aTask);
void DEBUG_SetTaskManagerEnabled(uint8_t aEnable);

static inline void SwitchContents(void) {
	static unsigned char i = 0;

	uint32_t tmpRunTime;

	if (i < (currentNumberOfTasks - 1)) {
		i++;
	} else {
		i = 0;
	}

	if (!task[i].suspend) {
		if (!task[i].useTimer) {
			if (*task[i].pTrigger) {
				tmpRunTime = micros;
				task[i].pTask();
				tmpRunTime = micros - tmpRunTime;
				task[i].RunTimeCnt += tmpRunTime;
				*task[i].pTrigger = false;
			}
		} else {
			if (TIMEOUT(task[i].TimeCnt, task[i].TimeOut)) {
				tmpRunTime = micros;
				task[i].pTask();
				tmpRunTime = micros - tmpRunTime;
				task[i].RunTimeCnt += tmpRunTime;
				task[i].TimeCnt = micros;
			}
		}
	}
}

static inline void TasksScheduler(void) {
	unsigned char i;
	uint32_t tmpRunTime;

	while (1) {
		for (i = 0; i < currentNumberOfTasks; i++) {
			if (!task[i].suspend) {
				if (!task[i].useTimer) {
					if (*task[i].pTrigger) {
						tmpRunTime = micros;
						task[i].pTask();
						tmpRunTime = micros - tmpRunTime;
						task[i].RunTimeCnt += tmpRunTime;
						*task[i].pTrigger = false;
					}
				} else {
					if (TIMEOUT(task[i].TimeCnt, task[i].TimeOut)) {
						tmpRunTime = micros;
						task[i].pTask();
						tmpRunTime = micros - tmpRunTime;
						task[i].RunTimeCnt += tmpRunTime;
						task[i].TimeCnt = micros;
					}
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
//task's error
#define TASK_ERROR_MAX_NUMBEER_OF_TASK_REACHED		-1
#define TASK_ERROR_NULL_TASK						-2

#endif	/* __TASK_H */

