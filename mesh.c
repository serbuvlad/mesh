#include "keyboard/keyboard.h"
#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/irq.h>
#include <hardware/pio.h>
#include <hardware/regs/intctrl.h>
#include <hardware/uart.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <screen/st7735.h>
#include <spi3wire.pio.h>
#include <screen/screen.h>
#include <keyboard/keyboard.h>



int main()
{
	stdio_init_all();

	keyboard_init();

	struct st7735 st7735;
	st7735_init(&st7735);

	struct screen screen;
	screen_init(&screen, &st7735);

	screen_print_cstr(&screen, "Intf Test\n\n");

	int i = 0;
	while (true) {
		while (keyboard_have_key()) {
			uint8_t c = keyboard_get_key();

			if (c == KEY_ENTER) {
				c = '\n';
			}

			if (c >= 1 && c < 0x80) {
				printf("Printing %x (%c)\n", c, c);
				screen_print_char(&screen, (char) c);
			}
		}
	}

/*
	stdio_init_all();

	keyboard_init();

	gpio_init(25);
	gpio_set_dir(25, true);

	uart_putc(uart0, 0xED);
	uart_putc(uart0, 4);

	bool a = true;

	while (true) {
		if (uart_is_readable(uart0)) {
			printf("Got char %hhx\n", uart_getc(uart0));
		} else {
			printf("Not available!!\n");
		}
      
		sleep_ms(1000);

	gpio_put(25, a);
	a = !a;
	}
  */
}
