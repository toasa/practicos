#include "terminal.h"
#include "vga.h"

void terminal_init(void) {
    t_row = 0;
    t_col = 0;
    t_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    // テキストバッファの開始アドレス
    t_buf = (uint16_t*) 0xB8000;
    
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            t_buf[index] = vga_entry(' ', t_color);
        }
    }
    terminal_writestring("Initialize terminal... OK\n");
}

void terminal_setcolor(uint8_t color) {
    t_color = color;
}

void terminal_putentryat(uint8_t c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    t_buf[index] = vga_entry(c, color);
}

void terminal_putchar(uint8_t c) {
    if (c == '\n') {
        t_col = 0;
        t_row++;
        return;
    }

    terminal_putentryat(c, t_color, t_col, t_row);
    t_col++;
    if (t_col >= VGA_WIDTH) {
        t_col = 0;
        t_row++;
    }
}

void terminal_write(const uint8_t *str, size_t len) {
    for (int i = 0; i < len; i++) {
        terminal_putchar(str[i]);
    }
}

void terminal_writestring(const uint8_t *str) {
    terminal_write(str, strlen(str));
}