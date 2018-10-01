/**
 ******************************************************************************
 * @file		ssd1306.h
 * @author		Milan Rakic
 * @date		21 September 2018
 ******************************************************************************
 */

#ifndef __SSD1306_H
#define __SSD1306_H

#ifdef __cplusplus
extern "C" {
#endif

/** Includes ---------------------------------------------------------------- */
//#include "stm32f10x_rcc.h"
#include "ssd1306_fonts.h"

// SSD1306 OLED height in pixels
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT          64
#endif

// SSD1306 width in pixels
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH           128
#endif

// some LEDs don't display anything in first two columns
// #define SSD1306_WIDTH           130

// Enumeration for screen colors
typedef enum {
    Black = 0x00, // Black color, no pixel
    White = 0x01  // Pixel is set. Color depends on OLED
} SSD1306_COLOR;

// Struct to store transformations
typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Inverted;
    uint8_t Initialized;
} SSD1306_t;

/** Public function prototypes ---------------------------------------------- */
void ssd1306_Init(void);
void ssd1306_Fill(SSD1306_COLOR color);
void ssd1306_UpdateScreen(void);
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color);
char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color);
void ssd1306_SetCursor(uint8_t x, uint8_t y);
void ssd1306_PrintTemperatures(char *tBoiler, char *tWaterHeater, char *tCollector);

#ifdef __cplusplus
}
#endif

#endif	/* __SSD1306_H */

