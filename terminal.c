#include "terminal.h"
#include "vga.h"

extern size_t prompt_len;

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

void terminal_uponerow(void) {
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_putentryat(t_buf[index], t_color, x, y - 1);
        }
    }

    t_row = VGA_HEIGHT - 1;
    terminal_clearonerow(t_row);
}

void terminal_clearonerow(size_t row) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_putentryat(' ', t_color, x, row);
    }
}

void terminal_putchar(uint8_t c) {
    if (c == '\n') {
        t_col = 0;
        t_row++;
        if (t_row >= VGA_HEIGHT) {
            terminal_uponerow();
        }
        return;
    } else if (c == '\b') {
        terminal_putentryat(' ', t_color, t_col - 1, t_row);
        if (t_col > prompt_len + 2) {
            t_col--;
            if (t_col <= prompt_len) {
                t_col = prompt_len;
            }
        }
        return;
    } else if (c == '\t') {
        t_col += 4;
    }

    terminal_putentryat(c, t_color, t_col, t_row);
    t_col++;
    if (t_col >= VGA_WIDTH) {
        t_col = 0;
        t_row++;
        if (t_row >= VGA_HEIGHT) {
            terminal_uponerow();
        }
    }
}

void terminal_putnum(int n, int base) {
    if (base == 10) {
        terminal_putchar(n + '0');
    } else if (base == 16) {
        if (n < 10) {
            terminal_putchar(n + '0');
        } else {
            terminal_putchar(n - 10 + 'A');
        }
    }
}

void terminal_write(const uint8_t *str, size_t len) {
    for (int i = 0; i < len; i++) {
        terminal_putchar(str[i]);
    }
}

void terminal_writenum_rec(int n, int base) {
    if (n > base - 1) {
        terminal_writenum_rec(n / base, base);
        terminal_putnum(n % base, base);
    } else {
        terminal_putnum(n, base);
    }
}

void terminal_writenum(int n, char base) {
    if (base == 'd') {
        terminal_writenum_rec(n, 10);
    } else if (base == 'x') {
        terminal_writenum_rec(n, 16);
    }
}

void terminal_writestring(const uint8_t *str) {
    terminal_write(str, sm_strlen(str));
}