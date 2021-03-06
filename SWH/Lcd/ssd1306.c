/**
 ******************************************************************************
 * @file		ssd1306.c
 * @author		Milan Rakic
 * @date		21 September 2018
 ******************************************************************************
 */

/** Includes ---------------------------------------------------------------- */
#include <math.h>
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"

#include "ssd1306.h"
#include "ssd1306_i2c.h"
#include "defines.h"
#include "systemTicks.h"
#include "mainController.h"
#include "nrf24_mid_level.h"

// Screenbuffer
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// Screen object
static SSD1306_t SSD1306;

/** Public functions -------------------------------------------------------- */
/**
 ******************************************************************************
 *	@brief	Initialize the OLED screen
 * @param	None
 * @retval	None
 ******************************************************************************
 */
void ssd1306_Init() {
	// Reset OLED
	ssd1306_Reset();

	// Wait for the screen to boot
	_DelayMS(100);

	// Init OLED
	ssd1306_WriteCommand(0xAE); //display off

	ssd1306_WriteCommand(0x20); //Set Memory Addressing Mode
	ssd1306_WriteCommand(0x10); // 00,Horizontal Addressing Mode; 01,Vertical Addressing Mode;
								// 10,Page Addressing Mode (RESET); 11,Invalid

	ssd1306_WriteCommand(0xB0); //Set Page Start Address for Page Addressing Mode,0-7

#ifdef SSD1306_MIRROR_VERT
	ssd1306_WriteCommand(0xC0); // Mirror vertically
#else
	ssd1306_WriteCommand(0xC8); //Set COM Output Scan Direction
#endif

	ssd1306_WriteCommand(0x00); //---set low column address
	ssd1306_WriteCommand(0x10); //---set high column address

	ssd1306_WriteCommand(0x40); //--set start line address - CHECK

	ssd1306_WriteCommand(0x81); //--set contrast control register - CHECK
	ssd1306_WriteCommand(0xFF);

#ifdef SSD1306_MIRROR_HORIZ
	ssd1306_WriteCommand(0xA0); // Mirror horizontally
#else
	ssd1306_WriteCommand(0xA1); //--set segment re-map 0 to 127 - CHECK
#endif

#ifdef SSD1306_INVERSE_COLOR
	ssd1306_WriteCommand(0xA7); //--set inverse color
#else
	ssd1306_WriteCommand(0xA6); //--set normal color
#endif

	ssd1306_WriteCommand(0xA8); //--set multiplex ratio(1 to 64) - CHECK
	ssd1306_WriteCommand(0x3F); //

	ssd1306_WriteCommand(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

	ssd1306_WriteCommand(0xD3); //-set display offset - CHECK
	ssd1306_WriteCommand(0x00); //-not offset

	ssd1306_WriteCommand(0xD5); //--set display clock divide ratio/oscillator frequency
	ssd1306_WriteCommand(0xF0); //--set divide ratio

	ssd1306_WriteCommand(0xD9); //--set pre-charge period
	ssd1306_WriteCommand(0x22); //

	ssd1306_WriteCommand(0xDA); //--set com pins hardware configuration - CHECK
	ssd1306_WriteCommand(0x12);

	ssd1306_WriteCommand(0xDB); //--set vcomh
	ssd1306_WriteCommand(0x20); //0x20,0.77xVcc

	ssd1306_WriteCommand(0x8D); //--set DC-DC enable
	ssd1306_WriteCommand(0x14); //
	ssd1306_WriteCommand(0xAF); //--turn on SSD1306 panel

	// Clear screen
	ssd1306_Fill(Black);

	// Flush buffer to screen
	ssd1306_UpdateScreen();

	// Set default values for screen object
	SSD1306.CurrentX = 0;
	SSD1306.CurrentY = 0;

	SSD1306.Initialized = 1;
}

// Fill the whole screen with the given color
void ssd1306_Fill(SSD1306_COLOR color) {
	/* Set memory */
	uint32_t i;

	for (i = 0; i < sizeof(SSD1306_Buffer); i++) {
		SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
	}
}

// Write the screenbuffer with changed to the screen
void ssd1306_UpdateScreen(void) {
	uint8_t i;
	for (i = 0; i < 8; i++) {
		ssd1306_WriteCommand(0xB0 + i);
		ssd1306_WriteCommand(0x00);
		ssd1306_WriteCommand(0x10);
		ssd1306_WriteData(&SSD1306_Buffer[SSD1306_WIDTH * i], SSD1306_WIDTH);
	}
}

//    Draw one pixel in the screenbuffer
//    X => X Coordinate
//    Y => Y Coordinate
//    color => Pixel color
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color) {
	if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
		// Don't write outside the buffer
		return;
	}

	// Check if pixel should be inverted
	if (SSD1306.Inverted) {
		color = (SSD1306_COLOR) !color;
	}

	// Draw in the right color
	if (color == White) {
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
	} else {
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
	}
}

// Draw 1 char to the screen buffer
// ch         => char om weg te schrijven
// Font     => Font waarmee we gaan schrijven
// color     => Black or White
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color) {
	uint32_t i, b, j;

	// Check remaining space on current line
	if (SSD1306_WIDTH <= (SSD1306.CurrentX + Font.FontWidth)
			|| SSD1306_HEIGHT <= (SSD1306.CurrentY + Font.FontHeight)) {
		// Not enough space on current line
		return 0;
	}

	// Use the font to write
	for (i = 0; i < Font.FontHeight; i++) {
		b = Font.data[(ch - 32) * Font.FontHeight + i];
		for (j = 0; j < Font.FontWidth; j++) {
			if ((b << j) & 0x8000) {
				ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i),
						(SSD1306_COLOR) color);
			} else {
				ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i),
						(SSD1306_COLOR) !color);
			}
		}
	}

	// The current space is now taken
	SSD1306.CurrentX += Font.FontWidth;

	// Return written char for validation
	return ch;
}

// Write full string to screenbuffer
char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color) {
	// Write until null-byte
	while (*str) {
		if (ssd1306_WriteChar(*str, Font, color) != *str) {
			// Char could not be written
			return *str;
		}

		// Next char
		str++;
	}

	// Everything ok
	return *str;
}

// Position the cursor
void ssd1306_SetCursor(uint8_t x, uint8_t y) {
	SSD1306.CurrentX = x;
	SSD1306.CurrentY = y;
}

// Print time on LCD
void ssd1306_PrintDateAndTime(char *date, char *time) {
	ssd1306_Fill(Black);

	ssd1306_SetCursor(18, 0);
	ssd1306_WriteString("DATUM I VREME", Font_7x10, White);
	ssd1306_SetCursor(18, 16);
	ssd1306_WriteString(date, Font_11x18, White);
	ssd1306_SetCursor(18, 38);
	ssd1306_WriteString(time, Font_11x18, White);

	ssd1306_UpdateScreen();
}

void ssd1306_DrawCircle(int16_t x0, int16_t y0, int16_t radius) {
	for (int16_t x = 0; x <= 2 * radius; x++) {
		for (int16_t y = 0; y <= 2 * radius; y++) {
			double distance = sqrt(
					(double) (x - radius) * (x - radius)
							+ (y - radius) * (y - radius));
			if (distance > radius - 0.5 && distance < radius + 0.5) {
				ssd1306_DrawPixel(x0 + x - radius, y0 + y - radius, White);
			}
		}
	}
}

void ssd1306_FillCircle(int16_t x0, int16_t y0, int16_t radius) {
	for (int16_t y = -radius; y <= radius; y++) {
		for (int16_t x = -radius; x <= radius; x++) {
			if (x * x + y * y <= radius * radius) {
				ssd1306_DrawPixel(x0 + x, y0 + y, White);
			}
		}
	}
}

// Print temperatures on LCD
void ssd1306_PrintTemperatures(char *tBoiler, char *tWaterHeater,
		char *tCollector) {
	ssd1306_Fill(Black);

	// Kotao
	ssd1306_SetCursor(92, 0);
	ssd1306_WriteString("KOTAO", Font_7x10, White);
	ssd1306_SetCursor(92, 16);
	ssd1306_WriteString(tBoiler, Font_11x18, White);
	if (m_boilerPump) {
		ssd1306_FillCircle(108, 48, 10);
	} else {
		ssd1306_DrawCircle(108, 48, 10);
	}

	// Bojler
	ssd1306_SetCursor(44, 0);
	ssd1306_WriteString("BOJLER", Font_7x10, White);
	ssd1306_SetCursor(40, 24);
	ssd1306_WriteString(tWaterHeater, Font_16x26, White);

	// Kolektor
	ssd1306_SetCursor(2, 0);
	ssd1306_WriteString("KOLEK", Font_7x10, White);
	ssd1306_SetCursor(2, 16);
	if (nrf24Data.connected) {
		ssd1306_WriteString(tCollector, Font_11x18, White);
		if (TIMEOUT(nrf24Data.timeout, NRF24_TIMEOUT)) {
			nrf24Data.connected = false;
		}
	} else {
		ssd1306_WriteString(" --", Font_11x18, White);
	}
	if (m_collectorPump) {
		ssd1306_FillCircle(16, 48, 10);
	} else {
		ssd1306_DrawCircle(16, 48, 10);
	}

	ssd1306_UpdateScreen();
}

