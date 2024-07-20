#include <stdint.h>
#include <graphics.h>

void terminal_cprint(uint16_t * string, uint32_t colour) {
	if (!background.fb) {
		return;
	}
	int position = txt_string_draw(string, background.cursor.x, background.cursor.y, colour, &background);
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
