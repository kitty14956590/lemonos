#include <stdint.h>
#include <graphics.h>

void terminal_cprint(uint16_t * string, uint32_t colour) {
	int char_width = root_window.size.width / 8; // width in chracters
	int position = txt_string_draw(string, root_window.cursor.x, root_window.cursor.y, colour, &root_window);
	root_window.cursor.x = position % char_width;
	root_window.cursor.y = position / char_width;
}

void terminal_cputc(uint16_t character, uint32_t colour) {
	terminal_cprint((uint16_t[2]) {character, u'\0'}, colour);
}

void terminal_print(uint16_t * string) {
	terminal_cprint(string, 15);
}

void terminal_putc(uint16_t character) {
	terminal_cputc(character, 15);
}
