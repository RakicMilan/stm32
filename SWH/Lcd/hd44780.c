/**
 ******************************************************************************
 * @file    hd44780.c
 * @author  Milan Rakic
 * @version V1.0.0
 * @date    21-August-2018
 * @brief   hd44780 library.
 *
 ******************************************************************************
 * <h2><center>&copy; COPYRIGHT 2018 MR</center></h2>
 ******************************************************************************
 */

#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include "stm32f10x.h"
#include "hd44780.h"
#include "systemTicks.h"

#define LCD_Port GPIOA

#define RS GPIO_Pin_0
#define EN GPIO_Pin_1

#define D4 GPIO_Pin_2
#define D5 GPIO_Pin_3
#define D6 GPIO_Pin_4
#define D7 GPIO_Pin_5

#define _ON 1
#define _OFF 0
#define _ONE_LINE 0
#define _TWO_LINES 1

#define CURSOR _OFF // Valid options are ON or OFF
#define BLINK _OFF // Valid option are ON or OFF
#define NUMLINES _TWO_LINES // Valid options are ONE_LINE or TWO_LINES
/**
 * Strobe EN pin
 */
void strobeEN(void) {
	GPIO_SetBits(LCD_Port, EN);
	_DelayUS(100);
	GPIO_ResetBits(LCD_Port, EN);
	_DelayUS(100);
}

/**
 * Used for 4-bit mode operation
 * @param nibble 4 bits of data to send to lcd
 */
void lcdNibble(uint8_t nibble) {
	if (nibble & 0x8)
		GPIO_SetBits(LCD_Port, D7);
	else
		GPIO_ResetBits(LCD_Port, D7);
	if (nibble & 0x4)
		GPIO_SetBits(LCD_Port, D6);
	else
		GPIO_ResetBits(LCD_Port, D6);
	if (nibble & 0x2)
		GPIO_SetBits(LCD_Port, D5);
	else
		GPIO_ResetBits(LCD_Port, D5);
	if (nibble & 0x1)
		GPIO_SetBits(LCD_Port, D4);
	else
		GPIO_ResetBits(LCD_Port, D4);

	strobeEN();
}

/**
 * Write a byte to the LCD
 * @param byte 8-bit data
 * @param type RS pin depends on data type if command or not
 */
void lcdWrite(uint8_t byte, LCD_REGISTER_TYPE type) {
	GPIO_ResetBits(LCD_Port, EN);
	if (type == COMMAND) // check expected data type
		GPIO_ResetBits(LCD_Port, RS); // sending special commands (see hd44780 datasheet)
	else
		GPIO_SetBits(LCD_Port, RS); // assume actual data

	lcdNibble((uint8_t)(byte >> 4)); // send higher 4-bits
	lcdNibble((uint8_t)(byte & 0x0F)); // send lower 4-bits
}

void initLCD(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	//Init GPIOs
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = EN | RS | D4 | D5 | D6 | D7;
	GPIO_ResetBits(LCD_Port, EN | RS | D4 | D5 | D6 | D7);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(LCD_Port, &GPIO_InitStructure);
	GPIO_ResetBits(LCD_Port, EN | RS | D4 | D5 | D6 | D7);

	_DelayMS(40);
	// wait 10ms for lcd internal initialization

	lcdNibble(0b0011); // Function set - Interface defaults to 8-bit mode
	_DelayMS(5);
	lcdNibble(0b0011); // Function set - Interface defaults to 8-bit mode
	_DelayMS(5);
	lcdNibble(0b0011); // Function set - Interface defaults to 8-bit mode
	_DelayMS(5);
	lcdNibble(0b0010); // Now set the interface to 4-bit mode
	_DelayMS(5);

	lcdWrite(((0b0010 << 4) | (NUMLINES << 3)), COMMAND); // 0x28 = 4-bit, 2-line, 5x8 font size
	_DelayMS(5);
	lcdWrite(((0b1100) | (CURSOR << 1) | BLINK), COMMAND); // 0x0C = Turn ON display, no cursor, no blinking
	_DelayMS(8);
	clearLCD(); // Clear the screen
	_DelayMS(5);
	lcdWrite(0x06, COMMAND); // Move from left-to-right, no shifting
	_DelayMS(5);
	clearLCD();

	GPIO_SetBits(LCD_Port, RS);
}

/**
 * Clears the LCD
 */
void clearLCD(void) {
	lcdWrite(LCD_CLEAR, COMMAND);
	_DelayMS(2);
}

void lcdChar(uint8_t y, uint8_t x, char Chr) {
	uint8_t data;
	switch (y) {
	case 1:
		data = (uint8_t)(127 + x);
		break;
	case 2:
		data = (uint8_t)(191 + x);
		break;
	case 3:
		data = (uint8_t)(147 + x);
		break;
	case 4:
		data = (uint8_t)(211 + x);
		break;
	default:
		break;
	}
	lcdWrite(data, COMMAND);
	lcdWrite(Chr, DATA);
}

void lcdPrint(uint8_t y, uint8_t x, char *buffer) {
	uint8_t data;
	switch (y) {
	case 1:
		data = (uint8_t)(127 + x);
		break;
	case 2:
		data = (uint8_t)(191 + x);
		break;
	case 3:
		data = (uint8_t)(147 + x);
		break;
	case 4:
		data = (uint8_t)(211 + x);
		break;
	default:
		break;
	}
	lcdWrite(data, COMMAND);
	while (*buffer) { // Write data to LCD up to null
		lcdWrite(*buffer++, DATA);
	}
}

//void lcdPrint2(uint8_t y, uint8_t x, const rom char *buffer) {
//    uint8_t data;
//    switch (y) {
//        case 1: data = 127 + x;
//            break;
//        case 2: data = 191 + x;
//            break;
//        case 3: data = 147 + x;
//            break;
//        case 4: data = 211 + x;
//            break;
//        default: break;
//    }
//    lcdWrite(data, COMMAND);
//    while (*buffer) { // Write data to LCD up to null
//        lcdWrite(*buffer++, DATA);
//    }
//}

void CreateCustomChar(char *customChar) {
	lcdWrite(0x40, COMMAND);
	while (*customChar) { // Write data to LCD up to null
		lcdWrite(*customChar++, DATA);
	}
}

// Print temperature on LCD
void hd44780_PrintTemperatures(char *tBoiler, char *tWaterHeater, char *tCollector) {
	uint8_t x = 1;
	lcdPrint(1, x + 1, (char *) "BOJ");
	lcdPrint(2, x, tWaterHeater);
	lcdChar(2, (uint8_t)(x + 3), 0x00); // Print degree Celsius on LCD

	x = 6;
	lcdPrint(1, x + 1, (char *) "KOT");
	lcdPrint(2, x, tBoiler);
	lcdChar(2, (uint8_t)(x + 3), 0x00); // Print degree Celsius on LCD

	x = 11;
	lcdPrint(1, x + 1, (char *) "KOL");
	lcdPrint(2, x, tCollector);
	lcdChar(2, (uint8_t)(x + 3), 0x00); // Print degree Celsius on LCD
}

