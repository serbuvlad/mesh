#ifndef SCREEN_SCREEN_H
#define SCREEN_SCREEN_H

#include <stddef.h>
#include <stdint.h>

#include <screen/st7735.h>

// width and height of screen in 8x8 characters
#define SCREEN_WIDTH 16
#define SCREEN_HEIGHT 20

// off-screen pixels at position 0; determined experminetally
#define SCREEN_PIXEL_X_OFFSET 0
#define SCREEN_PIXEL_Y_OFFSET 0

struct screen {
	struct st7735 *st7735;

	uint8_t x;
	uint8_t y;

	char lines[SCREEN_HEIGHT][SCREEN_WIDTH];
};

void screen_init(struct screen *screen, struct st7735 *st7735);

// Set all pixels to white
void screen_clear(struct screen *screen);

// Print character c to the screen at position (x, y)
// After the print x is incremented.
//   If the new x would be SCREEN_WIDTH, x is set to 0 and y is incremented.
//     If the new y would be SCREEN_HEIGHT, y is unchanged and screen_roll is called.
// 
// The following coltrol codes are interpreted:
//   '\n' -- set x to 0 and increment y
//   '\r' -- set x to 0
// 
// All other control codes are uninterpreted and will appear as spaces (because of the font).
void screen_print_char(struct screen *screen, char c);

// Print the characters of s until '\0' is reached.
void screen_print_cstr(struct screen *screen, const char *s);

// Print n characters from s. '\0' is uninterpreted and will be printed as a space (because of the font).
void screen_print_str(struct screen *screen, const char *s, size_t n);

// Scroll the screen. Moves every character to the line above it.
void screen_roll(struct screen *screen);

#endif // SCREEN_SCREEN_H
