#include <hardware/gpio.h>
#include <hardware/uart.h>
#include <keyboard/keyboard.h>
#include <stdbool.h>
#include <util/static_fifo.h>
#include <stdint.h>
#include <stdio.h>

DEFINE_STATIC_FIFO(
	uint8_t, // Data type of FIFO
	0,       // Zero for data type
	64,      // Size of FIFO
	keyboard_irq_fifo,
	keyboard_irq_fifo_read_it,
	keyboard_irq_fifo_write_it,
	keyboard_irq_fifo_has_data,
	keyboard_irq_fifo_get,
	keyboard_irq_fifo_put
)

static struct keyboard_state keyboard_state;

static void keyboard_state_init()
{
	keyboard_state.numlock = false;
	keyboard_state.capslock = false;
	keyboard_state.scrolllock = false;

	keyboard_state.lshift = false;
	keyboard_state.rshift = false;

	keyboard_state.key_state.e0 = false;
	keyboard_state.key_state.f0 = false;
	keyboard_state.key_state.e1 = 0;
}

static void keyboard_give_command(uint8_t command)
{
	uart_putc(uart0, command);
}

void keyboard_rx_irq()
{
	while (uart_is_readable(uart0)) {
		keyboard_irq_fifo_put(uart_getc(uart0));
	}
}

void keyboard_init()
{
	keyboard_state_init();
	uart_init(uart0, 9600);

	gpio_set_function(0, UART_FUNCSEL_NUM(uart0, 0));
	gpio_set_function(1, UART_FUNCSEL_NUM(uart0, 1));

	uart_set_hw_flow(uart0, false, false);
	uart_set_format(uart0, 8, 1, UART_PARITY_NONE);
	uart_set_fifo_enabled(uart0, false);
	uart_set_translate_crlf(uart0, false);

	irq_set_exclusive_handler(UART0_IRQ, keyboard_rx_irq);
	irq_set_enabled(UART0_IRQ, true);

	uart_set_irq_enables(uart0, true, false);
}

bool keyboard_have_key(void)
{
	return keyboard_irq_fifo_has_data();
}

uint8_t keyboard_get_key(void)
{
	uint8_t c = keyboard_process_byte(&keyboard_state, keyboard_irq_fifo_get());

	if (c == KEY_CAPS_LOCK || c == KEY_SCROLL_LOCK || c == KEY_NUM_LOCK) {
		uint8_t led_state =
			(((uint8_t) keyboard_state.capslock) << 2) |
			(((uint8_t) keyboard_state.numlock) << 1) |
			((uint8_t) keyboard_state.scrolllock);

		keyboard_give_command(KEYBCMD_SET_LED);
		keyboard_give_command(led_state);
	}

	return c;
}


