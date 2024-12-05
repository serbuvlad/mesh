#include "st7735.h"

#include <hardware/clocks.h>
#include <hardware/pio.h>
#include <hardware/pio_instructions.h>
#include <pico/stdlib.h>
#include <stdio.h>

#include <spi3wire.pio.h>

#include <assert.h>


static const uint clockPin = 6;
static const uint dataPin  = 7;
static const uint resetPin = 8;
static const uint dcPin    = 9;
static const uint csPin    = 10;

static const uint writeOffset      = 1;
static const uint burstWriteOffset = 10;
static const uint readOffset       = 17;

void st7735_init(struct st7735 *st7735)
{
    gpio_init(resetPin);
    gpio_set_dir(resetPin, true);

    gpio_init(dcPin);
    gpio_set_dir(dcPin, true);

    gpio_init(csPin);
    gpio_set_dir(csPin, true);

    gpio_put(resetPin, true);
    gpio_put(dcPin, true);
    gpio_put(csPin, true);

    PIO pio = pio0;
    int sm = pio_claim_unused_sm(pio, true);
    if (sm < 0) {
        printf("Error: could not claim state machine\n");
        return;
    }
    int offset = pio_add_program_at_offset(pio, &spi3wire_program, 0);
    if (offset < 0) {
        printf("Error: could not load program\n");
        return;
    }

    float div = (float) clock_get_hz(clk_sys) / 3000;
    spi3wire_program_init(pio, sm, offset, dataPin, clockPin, div);

    sleep_ms(120);
    gpio_put(resetPin, false);
    sleep_ms(120);
    gpio_put(resetPin, true);
    sleep_ms(120);

    st7735->pio = pio;
    st7735->sm = sm;
    st7735->offset = offset;

	st7735_reset(st7735);
}

static uint32_t st7735_read(struct st7735 *st, uint8_t command, bool dc, uint bits)
{
    gpio_put(dcPin, dc);
    gpio_put(csPin, false);

    uint set_instr = pio_encode_set(pio_x, bits - 1);
    pio_sm_exec_wait_blocking(st->pio, st->sm, set_instr);

    uint jmp_instr = pio_encode_jmp(st->offset + readOffset);
    pio_sm_exec_wait_blocking(st->pio, st->sm, jmp_instr);
    
    pio_sm_put_blocking(st->pio, st->sm, command);
    
    uint r = pio_sm_get_blocking(st->pio, st->sm);

    gpio_put(csPin, true);

    return r;
}

uint8_t st7735_read_8bit(struct st7735 *st, uint8_t command, bool dc)
{
    return (uint8_t) st7735_read(st, command, dc, 8);
}

uint32_t st7735_read_24bit(struct st7735 *st, uint8_t command, bool dc)
{
    // 25 because of a dummy clock cycle
    return st7735_read(st, command, dc, 25) & 0x00FFFFFF; // clear the dummy bit
}

uint32_t st7735_read_32bit(struct st7735 *st, uint8_t command, bool dc)
{
    // 33 because of a dummy clock cycle
    return st7735_read(st, command, dc, 33);
}

void st7735_write(struct st7735 *st, uint8_t command, bool dc)
{
    gpio_put(dcPin, dc);
    gpio_put(csPin, false);

    uint jmp_instr = pio_encode_jmp(st->offset + writeOffset);
    pio_sm_exec_wait_blocking(st->pio, st->sm, jmp_instr);
    
    pio_sm_put_blocking(st->pio, st->sm, command);

    (void) pio_sm_get_blocking(st->pio, st->sm);

    gpio_put(csPin, true);
}

void st7735_burst_write_words(struct st7735 *st, uint *words, uint n, bool dc)  
{
    gpio_put(dcPin, dc);
    gpio_put(csPin, false);

    uint set_instr = pio_encode_set(pio_x, n);
    pio_sm_exec_wait_blocking(st->pio, st->sm, set_instr);

    uint jmp_instr = pio_encode_jmp(st->offset + burstWriteOffset);
    pio_sm_exec_wait_blocking(st->pio, st->sm, jmp_instr);

    for (uint i = 0; i < n; i++) {
      pio_sm_put_blocking(st->pio, st->sm, words[i]);
    }

    (void) pio_sm_get_blocking(st->pio, st->sm);

    gpio_put(csPin, true);
}

void st7735_burst_write_bytes(struct st7735 *st, uint8_t *bytes, uint n, bool dc)  
{
    if (n % 4 != 0) {
        printf("Error: n is not a multiple of 4 in st7735_burst_write_bytes: %d\n", n);
        return;
    }

    gpio_put(dcPin, dc);
    gpio_put(csPin, false);

    uint set_instr = pio_encode_set(pio_x, n / 4);
    pio_sm_exec_wait_blocking(st->pio, st->sm, set_instr);

    uint jmp_instr = pio_encode_jmp(st->offset + burstWriteOffset);
    pio_sm_exec_wait_blocking(st->pio, st->sm, jmp_instr);

    for (uint i = 0; i < n; i += 4) {
      pio_sm_put_blocking(st->pio, st->sm, (bytes[i] << 24) | (bytes[i + 1] << 16) | (bytes[i + 2] << 8) | bytes[i + 3]);
    }

    gpio_put(csPin, true);
}

void st7735_reset(struct st7735 *st7735)
{
	// Software reset -- requires 120ms delay according to datasheet
	st7735_write(st7735, st7735_SWRESET, false);
	sleep_ms(120);

	// Display starts in sleep, get out of sleep -- requires 120ms delay
	st7735_write(st7735, st7735_SLPOUT, false);
	sleep_ms(120);

	// Display starts off, turn display on -- requires 120ms delay
	st7735_write(st7735, st7735_DISPON, false);
	sleep_ms(120);

  st7735_write(st7735, st7735_INVON, false);
}

void st7735_column_address_set(struct st7735 *st7735, uint16_t xs, uint16_t xe)
{
  printf("xs=%d xe=%d\n", xs, xe);

	st7735_write(st7735, st7735_CASET, false);

	// XS[15:8]
	st7735_write(st7735, xs >> 8, true);
	// XS[7:0]
	st7735_write(st7735, xs, true);
	// XE[15:8]
	st7735_write(st7735, xe >> 8, true);
	// XE[7:0]
	st7735_write(st7735, xe, true);
}

void st7735_row_address_set(struct st7735 *st7735, uint16_t ys, uint16_t ye)
{
  printf("ys=%d ye=%d\n", ys, ye);

	st7735_write(st7735, st7735_RASET, false);

	// YS[15:8]
	st7735_write(st7735, ys >> 8, true);
	// YS[7:0]
	st7735_write(st7735, ys, true);
	// YE[15:8]
	st7735_write(st7735, ye >> 8, true);
	// YE[7:0]
	st7735_write(st7735, ye, true);
}
