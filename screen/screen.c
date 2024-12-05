#include "screen/st7735.h"
#include <pico/stdlib.h>
#include <pico/time.h>
#include <screen/screen.h>
#include <screen/font.h>
#include <stdio.h>
#include <string.h>

void screen_init(struct screen *screen, struct st7735 *st7735)
{
	screen->st7735 = st7735;

	screen->x = 0;
	screen->y = 0;

	for (size_t i = 0; i < SCREEN_HEIGHT; i++) {
		for (size_t j = 0; j < SCREEN_WIDTH; j++) {
			screen->lines[i][j] = '\0';
		}
	}

	screen_clear(screen);
}

void screen_clear(struct screen *screen)
{
	st7735_write(screen->st7735, st7735_RAMWR, false);

	for (size_t i = 0; i < ST7735_HEIGHT; i++) {
		for (size_t j = 0; j < ST7735_WIDTH; j++) {
			for (size_t k = 0; k < 2 * 3; k++) {
				st7735_write(screen->st7735, 0, true);
			}
		}
	}
}

static inline void screen_render_char(struct screen *screen, char c, uint8_t xpos, uint8_t ypos)
{
	// Part 1 -- set columns and row addresses
	st7735_column_address_set(
		screen->st7735,
		ST7735_WIDTH - (((uint16_t) xpos + 1) * 8 - 1 + SCREEN_PIXEL_X_OFFSET),
		ST7735_WIDTH - (((uint16_t) xpos) * 8 + SCREEN_PIXEL_X_OFFSET)
	);
	st7735_row_address_set(
		screen->st7735,
		ST7735_HEIGHT - (((uint16_t) ypos + 1) * 8 - 1 + SCREEN_PIXEL_Y_OFFSET),
		ST7735_HEIGHT - (((uint16_t) ypos)     * 8 + SCREEN_PIXEL_Y_OFFSET)
	);

	// Part 2 -- render into array
	// 8x8 pixels; 3 bytes per pixel
	uint8_t renderedchar[8 * 8 * 3];
	size_t renderedchari = 0;

	for (int y = 7; y >= 0; y--) {
		uint8_t charrow = font[c][y];
		for (int x = 7; x >= 0; x--) {
			bool pixel = (charrow >> x) & 1;
			uint8_t color_value = pixel ? 0xFF : 0;
			
			// all three colors
			for (size_t k = 0; k < 3; k++) {
				renderedchar[renderedchari++] = color_value;
			}
		}
	}

	// Part 3 -- write to screen
	st7735_write(screen->st7735, st7735_RAMWR, false);
	// st7735_burst_write_bytes(screen->st7735, renderedchar, 8 * 8 * 3, true);
	for (size_t i = 0; i < 8 * 8 * 3; i++) {
		st7735_write(screen->st7735, renderedchar[i], true);
	}
}

void screen_print_char(struct screen *screen, char c)
{
	// Part 0 -- save in internal listing
	screen->lines[screen->y][screen->x] = c;

	screen_render_char(screen, ' ', screen->x, screen->y);

	// Part 1 -- render character
	if (c == '\r') {
		screen->x = 0;
	} else if (c == '\n') {
		screen->x = 0;
		if (++screen->y >= SCREEN_HEIGHT) {
			screen->y--;

			screen_roll(screen);
		}
	} else {
		screen_render_char(screen, c, screen->x, screen->y);

		// Part 2 -- modify x and y
		if (++screen->x >= SCREEN_WIDTH) {
			screen->x = 0;
			if (++screen->y >= SCREEN_HEIGHT) {
				screen->y--;

				screen_roll(screen);
			}
		}
	}

	screen_render_char(screen, '_', screen->x, screen->y);
}

void screen_print_cstr(struct screen *screen, const char *s)
{
	while (*s) {
		screen_print_char(screen, *s++);
	}
}

void screen_print_str(struct screen *screen, const char *s, size_t n)
{
	for (size_t i = 0; i < n; i++) {
		screen_print_char(screen, s[i]);
	}
}

void screen_roll(struct screen *screen)
{
	for (size_t y = 0; y < screen->y; y++) {
		memcpy(screen->lines[y], screen->lines[y + 1], SCREEN_WIDTH);
	}

	memset(screen->lines[screen->y], '\0', SCREEN_WIDTH);

	// Going backwards here looks better -- looks more like a scroll
	for (int8_t y = screen->y; y >= 0; y--) {
		for (size_t x = 0; x < SCREEN_WIDTH; x++) {
			screen_render_char(screen, screen->lines[y][x], x, y);
		}
	}
}
