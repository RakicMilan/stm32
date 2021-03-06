/**
 ******************************************************************************
 * @file    nrf24_mid_level.h
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    20-September-2018
 * @brief   nRF24 library.
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#ifndef __NRF24_MID_LEVEL_H
#define __NRF24_MID_LEVEL_H

#include "defines.h"

#define NRF24_TIMEOUT		10000000

typedef struct {
	uint8_t connected;
	uint32_t timeout;
} nrf24Data_t;

extern nrf24Data_t nrf24Data;
extern twoBytes m_tCollector;

void nRF24_Initialize(void);
void nRF24_Receive(void);

#endif	/* __NRF24_MID_LEVEL_H */

