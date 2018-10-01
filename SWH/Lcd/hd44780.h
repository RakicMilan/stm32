/**
 ******************************************************************************
 * @file    hd44780.h
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    21-August-2018
 * @brief   hd44780 library.
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#ifndef __HD44780_H
#define __HD44780_H

/////////////////////////////////////////////
//                                         //
// Available Lcd Commands                  //
//                                         //
/////////////////////////////////////////////

#define      LCD_FIRST_ROW           128
#define      LCD_SECOND_ROW          192
#define      LCD_THIRD_ROW           148
#define      LCD_FOURTH_ROW          212
#define      LCD_CLEAR               1
#define      LCD_RETURN_HOME         2
#define      LCD_CURSOR_OFF          12
#define      LCD_UNDERLINE_ON        14
#define      LCD_BLINK_CURSOR_ON     15
#define      LCD_MOVE_CURSOR_LEFT    16
#define      LCD_MOVE_CURSOR_RIGHT   20
#define      LCD_TURN_OFF            0
#define      LCD_TURN_ON             8
#define      LCD_SHIFT_LEFT          24
#define      LCD_SHIFT_RIGHT         28

typedef enum LcdRegisterType {
	COMMAND, DATA,
} LCD_REGISTER_TYPE;

extern char customChar[8];

void initLCD(void);
void clearLCD(void);
void lcdChar(uint8_t y, uint8_t x, char Chr);
void lcdPrint(uint8_t y, uint8_t x, char *buffer);
//void lcdPrint2(uint8_t y, uint8_t x, const rom char *buffer);
void CreateCustomChar(char *customChar);
void hd44780_PrintTemperatures(char *tBoiler, char *tWaterHeater, char *tCollector);

#endif  /* __HD44780_H */

