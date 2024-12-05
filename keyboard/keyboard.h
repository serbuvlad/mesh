#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>

enum {
	KEY_PRTSCR      = 0x80,
	KEY_PAUSE       = 0x81,
	KEY_WIN_LEFT    = 0x82,
	KEY_WIN_RIGHT   = 0x83,
	KEY_CTRL_LEFT   = 0x84,
	KEY_CTRL_RIGHT  = 0x85,
	KEY_ALT_LEFT    = 0x86,
	KEY_ALT_RIGHT   = 0x87,
	KEY_SHIFT_LEFT  = 0x88,
	KEY_SHIFT_RIGHT = 0x89,
	KEY_MENU        = 0x8A,
	KEY_CAPS_LOCK   = 0x8B,
	KEY_SCROLL_LOCK = 0x8C,
	KEY_NUM_LOCK    = 0x8D,
	KEY_ENTER       = 0x8E,
	KEY_ESC         = 0x8F,
	
	KEY_INSERT      = 0x90,
	KEY_DELETE      = 0x91,
	KEY_HOME        = 0x92,
	KEY_END         = 0x94,
	KEY_PAGE_UP     = 0x95,
	KEY_PAGE_DOWN   = 0x96,
	KEY_UP          = 0x97,
	KEY_DOWN        = 0x98,
	KEY_LEFT        = 0x99,
	KEY_RIGHT       = 0x9A,

	KEY_NUM_FIRST  = 0xA0,
	KEY_NUM_DOT     = 0xA0,
	KEY_NUM_ONE     = 0xA1,
	KEY_NUM_TWO     = 0xA2,
	KEY_NUM_THREE   = 0xA3,
	KEY_NUM_FOUR    = 0xA4,
	KEY_NUM_FIVE    = 0xA5,
	KEY_NUM_SIX     = 0xA6,
	KEY_NUM_SEVEN   = 0xA7,
	KEY_NUM_EIGHT   = 0xA8,
	KEY_NUM_NINE    = 0xA9,
	KEY_NUM_ZERO    = 0xAA,
	KEY_NUM_STAR    = 0xAB,
	KEY_NUM_PLUS    = 0xAC,
	KEY_NUM_MINUS   = 0xAD,
	KEY_NUM_SLASH   = 0xAE,
	KEY_NUM_ENTER   = 0xAF,
	KEY_NUM_LAST    = 0xAF,

	KEY_F1          = 0xB0,
	KEY_F2          = 0xB1,
	KEY_F3          = 0xB2,
	KEY_F4          = 0xB3,
	KEY_F5          = 0xB4,
	KEY_F6          = 0xB5,
	KEY_F7          = 0xB6,
	KEY_F8          = 0xB7,
	KEY_F9          = 0xB8,
	KEY_F10         = 0xB9,
	KEY_F11         = 0xBA,
	KEY_F12         = 0xBB,
};

enum {
	KEYBCMD_SET_LED = 0xED,
};

struct key_state {
	bool e0;
	bool f0;
	uint8_t e1;
};

struct keyboard_state {
	struct key_state key_state;

	bool numlock;
	bool capslock;
	bool scrolllock;
	bool lshift;
	bool rshift;
};

uint8_t keyboard_process_byte(struct keyboard_state *state, uint8_t byte);

void keyboard_init(void);
bool keyboard_have_key(void);
uint8_t keyboard_get_key(void);

#endif // KEYBOARD_H
