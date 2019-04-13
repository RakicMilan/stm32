/**
 ******************************************************************************
 * @file		ds1307_mid_level.h
 * @author		Milan Rakic
 * @date		10 February 2019
 ******************************************************************************
 */

#ifndef __DS1307_MID_LEVEL_H
#define __DS1307_MID_LEVEL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	TIME_SET_NONE = 0,
	TIME_SET_YEAR,
	TIME_SET_MONTH,
	TIME_SET_DATE,
	TIME_SET_HOURS,
	TIME_SET_MINUTES,
	TIME_SET_SECONDS
} SetTimeState_t;

typedef struct {
	uint8_t year;
	uint8_t month;
	uint8_t date;
	uint8_t hours_24;
	uint8_t minutes;
	uint8_t seconds;
} TimeStruct_t;

SetTimeState_t m_setTimeState;
extern TimeStruct_t SetTime;

void DisplayTime(void);

void InitSetTime(void);
void SetTimeNextStep(void);
void SetTimePreviousStep(void);
void IncreaseTime(void);
void DecreaseTime(void);

#ifdef __cplusplus
}
#endif

#endif /* __DS1307_MID_LEVEL_H */

/********************************* END OF FILE ********************************/
/******************************************************************************/
