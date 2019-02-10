/**
 ******************************************************************************
 * @file		ds1307_mid_level.c
 * @author		Milan Rakic
 * @date		10 February 2019
 ******************************************************************************
 */

/** Includes ---------------------------------------------------------------- */
#include <stdio.h>
#include "ds1307_mid_level.h"
#include "ds1307.h"
#include "ssd1306.h"

/** Public functions -------------------------------------------------------- */
void DisplayTime(void) {
	char date[] = "00:00:00";
	char time[] = "00:00:00";

	sprintf(date, "%02d:%02d:%02d", ds1307_get_date(), ds1307_get_month(), ds1307_get_year());
	sprintf(time, "%02d:%02d:%02d", ds1307_get_hours_24(), ds1307_get_minutes(), ds1307_get_seconds());

	ssd1306_PrintDateAndTime(date, time);
}

