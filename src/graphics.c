#include <graphics.h>
#include <multiboot.h>
#include <font.h>
#include <string.h>

int legacy_colour[16] = {
        0x000000,
        0x0000aa,
        0x00aa00,
        0x00aaaa,
        0xaa0000,
        0xaa00aa,
        0xaa5500,
        0xaaaaaa,
        0x555555,
        0x5555ff,
        0x55ff55,
        0x55ffff,
        0xff5555,
        0xff55ff,
        0xffff55,
        0xffffff
};

rect_2d_t root_window;

uint32_t font_get_character(uint16_t character) {
        for (uint32_t i = 0; i < font_size; i++) {
                if (((uint16_t) font[i][0]) == character) {
                        return i;
                }
        }
        return font_size - 1;
}

void font_truecolour_draw(uint32_t * fb, uint32_t chr, uint32_t colour, uint32_t position) {
        if (chr & 0xff) {
                fb[position] = (chr >> 8) | 0xff000000;
        }
}

void font_legacy_draw(uint32_t * fb, uint32_t chr, uint32_t colour, uint32_t position) {
        if (chr == 1) {
                fb[position] = colour;
        } else if (chr > 1) {
                fb[position] = legacy_colour[chr];
        }
}

void font_combining_draw(uint32_t * fb, uint32_t chr, uint32_t colour, uint32_t position) {
        font_legacy_draw(fb, chr, colour, position - 8);
}

void font_blank_draw(uint32_t * fb, uint32_t chr, uint32_t colour, uint32_t position) {
        fb[position] = 0;
}

font_drawer_t font_get_drawer(uint16_t character, uint32_t chr) {
        switch (font[chr][1]) {
                case FONT_LEGACY:
                        return font_legacy_draw;
                case FONT_BLANK:
                        return font_blank_draw;
                case FONT_COMBINING:
                        return font_combining_draw;
                case FONT_TRUECOLOUR:
                        return font_truecolour_draw;
        }
        return font_legacy_draw;
}

void gfx_char_draw(uint16_t character, int x, int y, uint32_t colour, rect_2d_t * rect) {
        uint32_t position = ((y * 16) * rect->size.width) + (x * 8);
        uint32_t chr = font_get_character(character); // cant call it char, GRAHHHH
        font_drawer_t drawer = font_get_drawer(character, chr);
        int line = (y * 16);
        int pixel = 0;
        for (int i = 0; i < 128; i++) {
                if (pixel == 8) {
                        line++; // go to the next line
                        position = (line * rect->size.width) + (x * 8);
                        pixel = 0; // go back to the start of the line
                }
                pixel++;
                drawer(rect->fb, font[chr][i + 2], colour, position + pixel);
        }
}

void gfx_scroll(rect_2d_t * rect) {
        memcpy32(rect->fb, rect->fb + (rect->size.width * 16), (rect->size.height - 16) * rect->size.width);
        memset32(rect->fb + ((rect->size.height - 16) * rect->size.width), 0, (rect->size.width * 16));
        rect->cursor.y--;
}

int gfx_string_draw(uint16_t * string, int x, int y, uint32_t colour, rect_2d_t * rect) {
        int position = x;
        int yoffset = 0;
        int char_width = rect->size.width / 8; // height and width in chracters
        int char_height = rect->size.height / 16;
        for (int i = 0; i < ustrlen(string); i++) {
                if (y + yoffset + position / char_width > char_height) {
                        gfx_scroll(rect);
                        yoffset--;
                }
                if (string[i] != u'\n') {
                        gfx_char_draw(string[i], position, y + yoffset, colour, rect);
                        if (font[font_get_character(string[i])][1] == FONT_COMBINING) {
                                position--;
                        }
                }
                if ((string[i] == u'\n') || (position + 1 == char_width)) {
                        position = -1;
                        yoffset++;
                }
                position++;
                if (position >= char_width) {
                        position = 0;
                        yoffset++;
                }
        }
        return ((y + yoffset) * char_width) + position;
}

int txt_string_draw(uint16_t * string, int x, int y, uint32_t colour, rect_2d_t * rect) {
        if (colour >= 16) {
                return gfx_string_draw(string, x, y, colour, rect);
        } else {
                return gfx_string_draw(string, x, y, legacy_colour[colour], rect);
        }
}

void gfx_init() {
	root_window.fb = (uint32_t *) (uint32_t) multiboot_header->framebuffer;
	root_window.cursor.x = 0;
	root_window.cursor.y = 0;
	root_window.x = 0;
	root_window.y = 0;
	root_window.size.width = multiboot_header->framebuffer_width;
	root_window.size.height = multiboot_header->framebuffer_height;
}
