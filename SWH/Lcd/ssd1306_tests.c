#include <stdio.h>
#include <string.h>
#include "ssd1306.h"
#include "systemTicks.h"

uint32_t GetTick() {
	return (uint32_t)(micros / 1000);
}

void ssd1306_TestBorder() {
	ssd1306_Fill(Black);

	uint32_t start = GetTick();
	uint32_t end = start;
	uint8_t x = 0;
	uint8_t y = 0;
	do {
		ssd1306_DrawPixel(x, y, Black);

		if ((y == 0) && (x < 127))
			x++;
		else if ((x == 127) && (y < 63))
			y++;
		else if ((y == 63) && (x > 0))
			x--;
		else
			y--;

		ssd1306_DrawPixel(x, y, White);
		ssd1306_UpdateScreen();

		_DelayMS(5);
		end = GetTick();
	} while ((end - start) < 8000);

	_DelayMS(1000);
}

void ssd1306_TestFonts() {
	ssd1306_Fill(Black);
	ssd1306_SetCursor(2, 0);
	ssd1306_WriteString("Font 16x26", Font_16x26, White);
	ssd1306_SetCursor(2, 26);
	ssd1306_WriteString("Font 11x18", Font_11x18, White);
	ssd1306_SetCursor(2, 26 + 18);
	ssd1306_WriteString("Font 7x10", Font_7x10, White);
	ssd1306_UpdateScreen();
}

void ssd1306_TestFPS() {
	ssd1306_Fill(White);

	uint32_t start = GetTick();
	uint32_t end = start;
	int fps = 0;
	char message[] = "ABCDEFGHIJK";

	ssd1306_SetCursor(2, 0);
	ssd1306_WriteString("Testing...", Font_11x18, Black);

	do {
		ssd1306_SetCursor(2, 18);
		ssd1306_WriteString(message, Font_11x18, Black);
		ssd1306_UpdateScreen();

		char ch = message[0];
		memmove(message, message + 1, sizeof(message) - 2);
		message[sizeof(message) - 2] = ch;

		fps++;
		end = GetTick();
	} while ((end - start) < 5000);

	_DelayMS(1000);

	char buff[64];
	fps = (float) fps / ((end - start) / 1000.0);
	snprintf(buff, sizeof(buff), "~%d FPS", fps);

	ssd1306_Fill(White);
	ssd1306_SetCursor(2, 18);
	ssd1306_WriteString(buff, Font_11x18, Black);
	ssd1306_UpdateScreen();
}

void ssd1306_TestAll() {
	ssd1306_Init();
	ssd1306_TestFPS();
	_DelayMS(1000);
	ssd1306_TestBorder();
	ssd1306_TestFonts();
}
