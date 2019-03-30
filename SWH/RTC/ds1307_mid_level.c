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
#include "debugUsart.h"

SetTimeFsm_t SetTime;

/** Public functions -------------------------------------------------------- */
void DisplayTime(void) {
	char date[] = "00:00:00";
	char time[] = "00:00:00";

	sprintf(date, "%02d.%02d.%02d", ds1307_get_date(), ds1307_get_month(),
			ds1307_get_year());
	sprintf(time, "%02d:%02d:%02d", ds1307_get_hours_24(), ds1307_get_minutes(),
			ds1307_get_seconds());

	ssd1306_PrintDateAndTime(date, time);
}

void PrintTime(void) {
	debug.printf("%d.%02d.%02d. %02d:%02d:%02d               \r\n",
			SetTime.year + 2000, SetTime.month, SetTime.date, SetTime.hours_24,
			SetTime.minutes, SetTime.seconds);
	debug.printf("Press n to proceed to the next step.               \r\n");
	DebugMoveCursorUp(2);
}

void InitSetTime(void) {
	SetTime.currentState = TIME_SET_YEAR;
	SetTime.year = ds1307_get_year();
	SetTime.month = ds1307_get_month();
	SetTime.date = ds1307_get_date();
	SetTime.hours_24 = ds1307_get_hours_24();
	SetTime.minutes = ds1307_get_minutes();
	SetTime.seconds = ds1307_get_seconds();

	PrintTime();
}

void SetTimeNextStep(void) {
	switch (SetTime.currentState) {
	case TIME_SET_YEAR:
		SetTime.currentState = TIME_SET_MONTH;
		break;
	case TIME_SET_MONTH:
		SetTime.currentState = TIME_SET_DATE;
		break;
	case TIME_SET_DATE:
		SetTime.currentState = TIME_SET_HOURS;
		break;
	case TIME_SET_HOURS:
		SetTime.currentState = TIME_SET_MINUTES;
		break;
	case TIME_SET_MINUTES:
		SetTime.currentState = TIME_SET_SECONDS;
		break;
	case TIME_SET_SECONDS:
		ds1307_set_year(SetTime.year);
		ds1307_set_month(SetTime.month);
		ds1307_set_date(SetTime.date);
		ds1307_set_hours_24(SetTime.hours_24);
		ds1307_set_minutes(SetTime.minutes);
		ds1307_set_seconds(SetTime.seconds);
		SetTime.currentState = TIME_SET_NONE;

		debug.printf("%d.%02d.%02d. %02d:%02d:%02d               \r\n",
				SetTime.year + 2000, SetTime.month, SetTime.date,
				SetTime.hours_24, SetTime.minutes, SetTime.seconds);
		debug.printf("Time is saved.                         \r\n");
		break;
	default:
		break;
	}
}

void SetTimePreviousStep(void) {
	switch (SetTime.currentState) {
	case TIME_SET_MONTH:
		SetTime.currentState = TIME_SET_YEAR;
		break;
	case TIME_SET_DATE:
		SetTime.currentState = TIME_SET_MONTH;
		break;
	case TIME_SET_HOURS:
		SetTime.currentState = TIME_SET_DATE;
		break;
	case TIME_SET_MINUTES:
		SetTime.currentState = TIME_SET_HOURS;
		break;
	case TIME_SET_SECONDS:
		SetTime.currentState = TIME_SET_MINUTES;
		break;
	default:
		break;
	}
}

void IncreaseTime(void) {
	switch (SetTime.currentState) {
	case TIME_SET_YEAR:
		if (SetTime.year < 99)
			SetTime.year++;
		else
			SetTime.year = 0;
		break;
	case TIME_SET_MONTH:
		if (SetTime.month < 12)
			SetTime.month++;
		else
			SetTime.month = 1;
		break;
	case TIME_SET_DATE:
		switch (SetTime.month) {
		case 2:
			if (SetTime.date < 28)
				SetTime.date++;
			else
				SetTime.date = 1;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			if (SetTime.date < 30)
				SetTime.date++;
			else
				SetTime.date = 1;
			break;
		default:
			if (SetTime.date < 31)
				SetTime.date++;
			else
				SetTime.date = 1;
			break;
		}
		break;
	case TIME_SET_HOURS:
		if (SetTime.hours_24 < 23)
			SetTime.hours_24++;
		else
			SetTime.hours_24 = 0;
		break;
	case TIME_SET_MINUTES:
		if (SetTime.minutes < 59)
			SetTime.minutes++;
		else
			SetTime.minutes = 0;
		break;
	case TIME_SET_SECONDS:
		if (SetTime.seconds < 59)
			SetTime.seconds++;
		else
			SetTime.seconds = 0;
		break;
	default:
		break;
	}
	PrintTime();
}

void DecreaseTime(void) {
	switch (SetTime.currentState) {
	case TIME_SET_YEAR:
		if (SetTime.year > 0)
			SetTime.year--;
		else
			SetTime.year = 99;
		break;
	case TIME_SET_MONTH:
		if (SetTime.month > 1)
			SetTime.month--;
		else
			SetTime.month = 12;
		break;
	case TIME_SET_DATE:
		if (SetTime.date > 1)
			SetTime.date--;
		else {
			switch (SetTime.month) {
			case 2:
				SetTime.date = 28;
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				SetTime.date = 30;
				break;
			default:
				SetTime.date = 31;
				break;
			}
		}
		break;
	case TIME_SET_HOURS:
		if (SetTime.hours_24 > 0)
			SetTime.hours_24--;
		else
			SetTime.hours_24 = 23;
		break;
	case TIME_SET_MINUTES:
		if (SetTime.minutes > 0)
			SetTime.minutes--;
		else
			SetTime.minutes = 59;
		break;
	case TIME_SET_SECONDS:
		if (SetTime.seconds > 0)
			SetTime.seconds--;
		else
			SetTime.seconds = 59;
		break;
	default:
		break;
	}
	PrintTime();
}

