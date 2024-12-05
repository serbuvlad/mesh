#include <stdint.h>
#include <keyboard/keyboard.h>
#include <stdbool.h>

struct sm_state {
	bool e0;
	bool f0;
	uint8_t e1;
};

static uint16_t process_key(struct key_state *state, uint8_t byte);
static uint8_t uppercase(uint8_t c, bool letters, bool numsyms);
static uint8_t numkey(uint8_t c, bool numlock);

uint8_t keyboard_process_byte(struct keyboard_state *state, uint8_t byte)
{
	uint16_t bc = process_key(&state->key_state, byte);

	uint8_t b = bc >> 8;
	uint8_t c = bc;

	if (c == 0) {
		return 0;
	}

	// break code
	if (b) {
		// only care about shifts
		switch (c) {
		case KEY_SHIFT_LEFT:
			state->lshift = false;
			return 0;
		case KEY_SHIFT_RIGHT:
			state->rshift = false;
			return 0;
		default:
			return 0;
		}
	} else {
		// if it's a lock
		if (c == KEY_CAPS_LOCK) {
			state->capslock = !state->capslock;
		} else if (c == KEY_NUM_LOCK) {
			state->numlock = !state->numlock;
		} else if (c == KEY_SCROLL_LOCK) {
			state->scrolllock = !state->scrolllock;
		}

		// if it's a shfit
		if (c == KEY_SHIFT_LEFT) {
			state->lshift = true;
		} else if (c == KEY_SHIFT_RIGHT) {
			state->rshift = true;
		}

		// uppercase depending on CAPS or SHIFT
		//  CAPS && !SHIFT -> only letters
		//  CAPS &&  SHIFT -> only numbers & symbols
		// !CAPS &&  SHIFT -> numbers, letters & symbols
		if (state->capslock && !state->lshift && !state->rshift) {
			c = uppercase(c, true, false);
		} else if (state->capslock && (state->lshift || state->rshift)) {
			c = uppercase(c, false, true);
		} else if (!state->capslock && (state->lshift || state->rshift)) {
			c = uppercase(c, true, true);
		}

		// Process numpad keys
		if (c >= KEY_NUM_FIRST && c <= KEY_NUM_LAST) {
			c = numkey(c, state->numlock);
		}

		return c;
	}
}
 
static uint16_t process_key(struct key_state *state, uint8_t byte)
{
	if (state->e1) {
		state->e1--;
		return 0;
	}
 
	if (byte == 0xE1) {
		state->e1 = 2;
		return 0;
	}
 
	if (byte == 0xE0) {
		state->e0 = true;
	return 0;
	}
 
	if (byte == 0xF0) {
		state->f0 = true;
		return 0;
	}
 
	uint16_t c = state->f0 ? (1 << 8) : 0;
 
	if (state->e0) {
		switch (byte) {
		// Print screen also generates E07C but we can ignore it
		case 0x12:
			c |= KEY_PRTSCR;
			break;
		// Print screen also generates E11477E1F014 but we can ignore it
		case 0x77:
			c |= KEY_PAUSE;
			break;
		case 0x1F:
			c |= KEY_WIN_LEFT;
			break;
		case 0x11:
			c |= KEY_ALT_RIGHT;
			break;
		case 0x27:
			c |= KEY_WIN_RIGHT;
			break;
		case 0x2F:
			c |= KEY_MENU;
			break;
		case 0x14:
			c |= KEY_CTRL_RIGHT;
			break;
		case 0x70:
			c |= KEY_INSERT;
			break;
		case 0x6C:
			c |= KEY_HOME;
			break;
		case 0x7D:
			c |= KEY_PAGE_UP;
			break;
		case 0x71:
			c |= KEY_DELETE;
			break;
		case 0x69:
			c |= KEY_END;
			break;
		case 0x7A:
			c |= KEY_PAGE_DOWN;
			break;
		case 0x75:
			c |= KEY_UP;
			break;
		case 0x6B:
			c |= KEY_LEFT;
			break;
		case 0x72:
			c |= KEY_DOWN;
			break;
		case 0x74:
			c |= KEY_RIGHT;
			break;
		case 0x4A:
			c |= KEY_NUM_SLASH;
			break;
		case 0x5A:
			c |= KEY_NUM_ENTER;
			break;
		default:
			c = 0;
			break;
		}
	} else {
		switch (byte) {
		case 0x76:
			c |= KEY_ESC;
			break;
		case 0x05:
			c |= KEY_F1;
			break;
		case 0x06:
			c |= KEY_F2;
			break;
		case 0x04:
			c |= KEY_F3;
			break;
		case 0x0C:
			c |= KEY_F4;
			break;
		case 0x03:
			c |= KEY_F5;
			break;
		case 0x0B:
			c |= KEY_F6;
			break;
		case 0x83:
			c |= KEY_F7;
			break;
		case 0x0A:
			c |= KEY_F8;
			break;
		case 0x01:
			c |= KEY_F9;
			break;
		case 0x09:
			c |= KEY_F10;
			break;
		case 0x78:
			c |= KEY_F11;
			break;
		case 0x07:
			c |= KEY_F12;
			break;
		case 0x7E:
			c |= KEY_SCROLL_LOCK;
			break;
		case 0x0E:
			c |= '`';
			break;
		case 0x16:
			c |= '1';
			break;
		case 0x1E:
			c |= '2';
			break;
		case 0x26:
			c |= '3';
			break;
		case 0x25:
			c |= '4';
			break;
		case 0x2E:
			c |= '5';
			break;
		case 0x36:
			c |= '6';
			break;
		case 0x3D:
			c |= '7';
			break;
		case 0x3E:
			c |= '8';
			break;
		case 0x46:
			c |= '9';
			break;
		case 0x45:
			c |= '0';
			break;
		case 0x4E:
			c |= '-';
			break;
		case 0x55:
			c |= '=';
			break;
		case 0x66:
			c |= '\b';
			break;
		case 0x0D:
			c |= '\t';
			break;
		case 0x15:
			c |= 'q';
			break;
		case 0x1D:
			c |= 'w';
			break;
		case 0x24:
			c |= 'e';
			break;
		case 0x2D:
			c |= 'r';
			break;
		case 0x2C:
			c |= 't';
			break;
		case 0x35:
			c |= 'y';
			break;
		case 0x3C:
			c |= 'u';
			break;
		case 0x43:
			c |= 'i';
			break;
		case 0x44:
			c |= 'o';
			break;
		case 0x4D:
			c |= 'p';
			break;
		case 0x54:
			c |= '[';
			break;
		case 0x5B:
			c |= ']';
			break;
		case 0x5D:
			c |= '\\';
			break;
		case 0x58:
			c |= KEY_CAPS_LOCK;
			break;
		case 0x1C:
			c |= 'a';
			break;
		case 0x1B:
			c |= 's';
			break;
		case 0x23:
			c |= 'd';
			break;
		case 0x2B:
			c |= 'f';
			break;
		case 0x34:
			c |= 'g';
			break;
		case 0x33:
			c |= 'h';
			break;
		case 0x3B:
			c |= 'j';
			break;
		case 0x42:
			c |= 'k';
			break;
		case 0x4B:
			c |= 'l';
			break;
		case 0x4C:
			c |= ';';
			break;
		case 0x52:
			c |= '\'';
			break;
		case 0x5A:
			c |= KEY_ENTER;
			break;
		case 0x12:
			c |= KEY_SHIFT_LEFT;
			break;
		case 0x1A:
			c |= 'z';
			break;
		case 0x22:
			c |= 'x';
			break;
		case 0x21:
			c |= 'c';
			break;
		case 0x2A:
			c |= 'v';
			break;
		case 0x32:
			c |= 'b';
			break;
		case 0x31:
			c |= 'n';
			break;
		case 0x3A:
			c |= 'm';
			break;
		case 0x41:
			c |= ',';
			break;
		case 0x49:
			c |= '.';
			break;
		case 0x4A:
			c |= '/';
			break;
		case 0x59:
			c |= KEY_SHIFT_RIGHT;
			break;
		case 0x14:
			c |= KEY_CTRL_LEFT;
			break;
		case 0x11:
			c |= KEY_ALT_LEFT;
			break;
		case 0x29:
			c |= ' ';
			break;
		case 0x77:
			c |= KEY_NUM_LOCK;
			break;
		case 0x7C:
			c |= KEY_NUM_STAR;
			break;
		case 0x7B:
			c |= KEY_NUM_MINUS;
			break;
		case 0x6C:
			c |= KEY_NUM_SEVEN;
			break;
		case 0x75:
			c |= KEY_NUM_EIGHT;
			break;
		case 0x7D:
			c |= KEY_NUM_NINE;
			break;
		case 0x79:
			c |= KEY_NUM_PLUS;
			break;
		case 0x6B:
			c |= KEY_NUM_FOUR;
			break;
		case 0x73:
			c |= KEY_NUM_FIVE;
			break;
		case 0x74:
			c |= KEY_NUM_SIX;
			break;
		case 0x69:
			c |= KEY_NUM_ONE;
			break;
		case 0x72:
			c |= KEY_NUM_TWO;
			break;
		case 0x7A:
			c |= KEY_NUM_THREE;
			break;
		case 0x70:
			c |= KEY_NUM_ZERO;
			break;
		case 0x71:
			c |= KEY_NUM_DOT;
			break;
		default:
			c = 0;
			break;
		}
	}

	state->f0 = false;
	state->e0 = false;

	return c;
}

static uint8_t uppercase(uint8_t c, bool letters, bool numsyms)
{
	if (c >= 'a' && c <= 'z' && letters) {
		return c - 32;
	} else if (numsyms) {
		switch (c) {
		case '`':
			return '~';
		case '1':
			return '!';
		case '2':
			return '@';
		case '3':
			return '#';
		case '4':
			return '$';
		case '5':
			return '%';
		case '6':
			return '^';
		case '7':
			return '&';
		case '8':
			return '*';
		case '9':
			return '(';
		case '0':
			return ')';
		case '-':
			return '_';
		case '=':
			return '+';
		case '[':
			return '{';
		case ']':
			return '}';
		case ';':
			return ':';
		case '\'':
			return '"';
		case '\\':
			return '|';
		case ',':
			return '<';
		case '.':
			return '>';
		case '/':
			return '?';
		}
	}

	return c;
}

static uint8_t numkey(uint8_t c, bool numlock)
{
	if (numlock) {
		switch (c) {
		case KEY_NUM_DOT:
			return '.';
		case KEY_NUM_ONE:
			return '1';
		case KEY_NUM_TWO:
			return '2';
		case KEY_NUM_THREE:
			return '3';
		case KEY_NUM_FOUR:
			return '4';
		case KEY_NUM_FIVE:
			return '5';
		case KEY_NUM_SIX:
			return '6';
		case KEY_NUM_SEVEN:
			return '7';
		case KEY_NUM_EIGHT:
			return '8';
		case KEY_NUM_NINE:
			return '9';
		case KEY_NUM_ZERO:
			return '0';
		case KEY_NUM_STAR:
			return '*';
		case KEY_NUM_PLUS:
			return '+';
		case KEY_NUM_MINUS:
			return '-';
		case KEY_NUM_SLASH:
			return '/';
		case KEY_NUM_ENTER:
			return KEY_ENTER;
		default:
			return 0;
		}
	} else {
		switch (c) {
		case KEY_NUM_EIGHT:
			return KEY_UP;
		case KEY_NUM_FOUR:
			return KEY_LEFT;
		case KEY_NUM_TWO:
			return KEY_DOWN;
		case KEY_NUM_SIX:
			return KEY_RIGHT;
		default:
			return 0;
		}
	}
}
