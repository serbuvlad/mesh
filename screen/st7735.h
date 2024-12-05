#ifndef ST7735_H
#define ST7735_H

#include <pico/stdlib.h>
#include <spi3wire.pio.h>
#include <stdint.h>

struct st7735 {
    PIO   pio;
    uint  sm;
    uint  offset;
};

void st7735_init(struct st7735 *st7735);

void     st7735_write(struct st7735 *st7735, uint8_t byte, bool dc);
void     st7735_burst_write_words(struct st7735 *st7735, uint *words, uint n, bool dc);
void     st7735_burst_write_bytes(struct st7735 *st7735, uint8_t *bytes, uint n, bool dc);

uint8_t  st7735_read_8bit(struct st7735 *st7735, uint8_t byte, bool dc);
uint32_t st7735_read_24bit(struct st7735 *st7735, uint8_t byte, bool dc);
uint32_t st7735_read_32bit(struct st7735 *st7735, uint8_t byte, bool dc);

void st7735_reset(struct st7735 *st7735);
void st7735_column_address_set(struct st7735 *st7735, uint16_t xs, uint16_t xe);
void st7735_row_address_set(struct st7735 *st7735, uint16_t ys, uint16_t ye);

#define ST7735_WIDTH 128
#define ST7735_HEIGHT 160

enum {
    st7735_NOP     = 0x00,
    st7735_SWRESET = 0x01,
    st7735_SLPOUT  = 0x11,
    st7735_INVON   = 0x21,
    st7735_DISPON  = 0x29,
    st7735_CASET   = 0x2A,
    st7735_RASET   = 0x2B,
    st7735_RAMWR   = 0x2C,
  st7735_RDID1   = 0xDA,
    st7735_RDID2   = 0xDB,
    st7735_RDID3   = 0xDC,
};

#endif // ST7735_H
