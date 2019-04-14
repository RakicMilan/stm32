/**
 ******************************************************************************
 * @file    mainController.h
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    13-Aprpil-2019
 * @brief   main controller
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2019 MR</center></h2>
 ******************************************************************************
 */

#ifndef __MAINCONTROLLER_H
#define	__MAINCONTROLLER_H

extern uint8_t m_boilerPump;
extern uint8_t m_collectorPump;

void InitWaterPump(void);
void WaterPumpController(void);
void LoadParameters(void);

#endif	/* __MAINCONTROLLER_H */

