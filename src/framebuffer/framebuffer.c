#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../header/text/framebuffer.h"
#include "../header/stdlib/string.h"
#include "../header/cpu/portio.h"

void framebuffer_set_cursor(uint8_t r, uint8_t c) {
    uint16_t pos = c + r*80;

    out(CURSOR_PORT_CMD, 0x0F);
	out(CURSOR_PORT_DATA, (uint8_t) (pos & 0xFF));
	out(CURSOR_PORT_CMD, 0x0E);
	out(CURSOR_PORT_DATA, (uint8_t) ((pos >> 8) & 0xFF));
}

void framebuffer_write(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg) {
    uint16_t attrib = (bg << 4) | (fg & 0x0F);
    volatile uint16_t * where;
    where = ((uint16_t*) FRAMEBUFFER_MEMORY_OFFSET) + (row * 80 + col) ;
    *where = c | (attrib << 8);
}

void framebuffer_clear(void) {
    // uint16_t clear = ' ' | (0x07 << 8);
    // for(int i = 0; i < 25; i++){
    //     for(int j = 0; j < 80; j++){
    //         FRAMEBUFFER_MEMORY_OFFSET[i * 80 + j] = clear;
    //     }
    // }
    uint16_t clear = 0x00 | (0x07 << 8);
    for(int i = 0; i < 25; i++){
        memset(FRAMEBUFFER_MEMORY_OFFSET + i*80, clear, 160);
    }
}
