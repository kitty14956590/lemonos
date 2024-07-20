#include <graphics.h>
#include <multiboot.h>
#include <memory.h>
#include <font.h>
#include <string.h>
#include <linked.h>

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
rect_2d_t back_buffer;
rect_2d_t background;
rect_2d_t taskbar;
rect_2d_t cursor;
linked_t * taskbar_buttons;
taskbar_button_t startbutton;
taskbar_button_t testbutton;
uint32_t button_offset;
uint32_t button_border = 0xff000000;
int taskbar_height = 28;
int gfx_init_done = 0;
int taskbar_updated = 0;

uint32_t background_colour = 0xff5500aa;
uint32_t taskbar_colour = 0xffaaaaaa;

uint32_t font_get_character(uint16_t character) {
	for (uint32_t i = 0; i < font_size; i++) {
		if (((uint16_t) font[i][0]) == character) {
			return i;
		}
	}
	return font_size - 1;
}

void font_truecolour_draw(uint32_t * fb, uint32_t chr, uint32_t colour, uint32_t position) {
	if (chr & 0xff000000) {
		fb[position] = chr | 0xff000000;
	}
}

void font_legacy_draw(uint32_t * fb, uint32_t chr, uint32_t colour, uint32_t position) {
	if (chr == 1) {
		fb[position] = colour | 0xff000000;
	} else if (chr > 1) {
		fb[position] = legacy_colour[chr] | 0xff000000;
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
		if (position > (rect->size.height * rect->size.width)) {
			continue;
		}
		drawer(rect->fb, font[chr][i + 2], colour, position + pixel);
	}
}

void gfx_char_p_draw(uint16_t character, int x, int y, uint32_t colour, rect_2d_t * rect) {
	uint32_t position = (y * rect->size.width) + x;
	uint32_t chr = font_get_character(character); // cant call it char, GRAHHHH
	font_drawer_t drawer = font_get_drawer(character, chr);
	int line = y;
	int pixel = 0;
	for (int i = 0; i < 128; i++) {
		if (pixel == 8) {
			line++; // go to the next line
			position = (line * rect->size.width) + x;
			pixel = 0; // go back to the start of the line
		}
		pixel++;
		drawer(rect->fb, font[chr][i + 2], colour, position + pixel);
	}
}

void gfx_scroll(rect_2d_t * rect) {
	memcpy32(rect->fb, rect->fb + (rect->size.width * 16), (rect->size.height - 16) * rect->size.width);
	memset32(rect->fb + ((rect->size.height - 16) * rect->size.width), background_colour, (rect->size.width * 16));
}

int gfx_string_draw(uint16_t * string, int x, int y, int p, uint32_t colour, rect_2d_t * rect) {
	int position = x;
	int yoffset = 0;
	int char_width = (int) (rect->size.width / 8); // height and width in chracters
	int char_height = (int) (rect->size.height / 16);
	for (int i = 0; i < ustrlen(string); i++) {
		if (y + (yoffset + 1) + position / char_width > char_height) {
			gfx_scroll(rect);
			yoffset--;
		}
		if (string[i] != u'\n') {
			if (p) {
				gfx_char_p_draw(string[i], position, y + yoffset, colour, rect);
			} else {
				gfx_char_draw(string[i], position, y + yoffset, colour, rect);
			}
			if (font[font_get_character(string[i])][1] == FONT_COMBINING) {
					position--;
			}
		}
		position++;
		if (string[i] == u'\n' || position >= char_width) {
			position = 0;
			yoffset++;
		}
	}
	rect->cursor.x = position % char_width;
	rect->cursor.y = y + yoffset;
}

int txt_string_draw(uint16_t * string, int x, int y, uint32_t colour, rect_2d_t * rect) {
	if (colour >= 16) {
		return gfx_string_draw(string, x, y, 0, colour, rect);
	} else {
		return gfx_string_draw(string, x, y, 0, legacy_colour[colour], rect);
	}
}

int txt_string_p_draw(uint16_t * string, int x, int y, uint32_t colour, rect_2d_t * rect) {
	if (colour >= 16) {
		return gfx_string_draw(string, x, y, 1, colour, rect);
	} else {
		return gfx_string_draw(string, x, y, 1, legacy_colour[colour], rect);
	}
}

// alpha_calculate() + rect_2d_adraw() are from LemonOS for linux
uint32_t alpha_calculate(uint32_t top, uint32_t bottom) {
	float alpha = (float) (top >> 24 & 0xff) / 255;
	float invert = 1 - alpha;
	uint8_t output[4];
	uint8_t top_r = (top >> 16) & 0xff;
	uint8_t top_g = (top >> 8) & 0xff;
	uint8_t top_b = top & 0xff;
	uint8_t bottom_r = (bottom >> 16) & 0xff;
	uint8_t bottom_g = (bottom >> 8) & 0xff;
	uint8_t bottom_b = bottom & 0xff;
	output[3] = 0xff;
	output[2] = (uint32_t) ((alpha * top_r) + (invert * bottom_r));
	output[1] = (uint32_t) ((alpha * top_g) + (invert * bottom_g));
	output[0] = (uint32_t) ((alpha * top_b) + (invert * bottom_b));
	return *((uint32_t *) output);
}


void rect_2d_draw(rect_2d_t * rect2, rect_2d_t * rect) {
	uint32_t * dest;
	const uint32_t * src;
	for (int i = 0; i < rect->size.height; i++) {
		if ((i + rect->y) < 0) {
			continue;
		}
		if ((i + rect->y) >= rect2->size.height) {
			break;
		}
		dest = (uint32_t *) (((void *) rect2->fb) + (((rect->y + i) * (rect2->size.width * 4)) + (rect->x * 4)));
		src = (uint32_t *) (((void *) rect->fb) + (i * (rect->size.width * 4)));
		for (int b = 0; b < rect->size.width; b++) {
			if ((b + rect->x) < 0) {
				continue;
			}
			if ((b + rect->x) >= rect2->size.width) {
				break;
			}
			*(dest + b) = *(src + b);
		}
	}
	return;
}

void rect_2d_adraw(rect_2d_t * rect2, rect_2d_t * rect) {
	uint32_t * dest;
	const uint32_t * src;
	for (int i = 0; i < rect->size.height; i++) {
		if ((i + rect->y) < 0) {
			continue;
		}
		if ((i + rect->y) >= rect2->size.height) {
			break;
		}
		dest = (uint32_t *) (((void *) rect2->fb) + (((rect->y + i) * (rect2->size.width * 4)) + (rect->x * 4)));
		src = (uint32_t *) (((void *) rect->fb) + (i * (rect->size.width * 4)));
		for (int b = 0; b < rect->size.width; b++) {
			if ((b + rect->x) < 0) {
				continue;
			}
			if ((b + rect->x) >= rect2->size.width) {
				break;
			}
			*(dest + b) = alpha_calculate(*(src + b), *(dest + b));
		}
	}
	return;
}

// fast transparent draw
void rect_2d_afdraw(rect_2d_t * rect2, rect_2d_t * rect) {
	uint32_t * dest;
	const uint32_t * src;
	for (int i = 0; i < rect->size.height; i++) {
		if ((i + rect->y) < 0) {
			continue;
		}
		if ((i + rect->y) >= rect2->size.height) {
			break;
		}
		dest = (uint32_t *) (((void *) rect2->fb) + (((rect->y + i) * (rect2->size.width * 4)) + (rect->x * 4)));
		src = (uint32_t *) (((void *) rect->fb) + (i * (rect->size.width * 4)));
		for (int b = 0; b < rect->size.width; b++) {
			if ((b + rect->x) < 0) {
				continue;
			}
			if ((b + rect->x) >= rect2->size.width) {
				break;
			}
			if (!((*(src + b)) >> 24)) {
				continue;
			}
			*(dest + b) = *(src + b);
		}
	}
	return;
}

uint32_t draw_taskbar_button(taskbar_button_t * button, uint32_t x) {
	uint16_t * p = button->text;
	int i = 0;
	int len = ustrlen(p);
	rect_2d_t icon = {
		6 + x + (i * 8), 6, 0, {0, 0},
		button->icon,
		{16, 16}
	};

	rect_2d_adraw(&taskbar, &icon);

	while (i < len) {
		gfx_char_p_draw(*p++, 28 + x + (i * 8), 6, 0, &taskbar);
		i++;
	}

	// todo: explain why these random numbers are here
	memset32(taskbar.fb + 3 + x + (3 * taskbar.size.width), button_border, 16 + (len * 8) + 9 + 4);
	memset32(taskbar.fb + 3 + x + (24 * taskbar.size.width), button_border, 16 + (len * 8) + 9 + 4);
	for (int i = 23; i > 3; i--) {
		taskbar.fb[3 + x + (i * taskbar.size.width)] = button_border;
		taskbar.fb[6 + 16 + (len * 8) + 9 + x + (i * taskbar.size.width)] = button_border;
	}
	return 16 + (len * 8) + 6 + 9;
}

void draw_taskbar_callback(linked_t * node) {
	taskbar_button_t * button = node->p;
	button_offset += draw_taskbar_button(button, button_offset);
}

void draw_taskbar_buttons() {
	if (!taskbar_updated) {
		return;
	}
	button_offset = 0;
	memset32(taskbar.fb, taskbar_colour, root_window.size.width * taskbar_height);
	linked_iterate(taskbar_buttons, draw_taskbar_callback);
	taskbar_updated = 0;
}

void draw_frame() {
	if (!gfx_init_done) {
		return;
	}
	draw_taskbar_buttons();
	rect_2d_draw(&back_buffer, &background);
	rect_2d_draw(&back_buffer, &taskbar);
	rect_2d_afdraw(&back_buffer, &cursor);
	rect_2d_draw(&root_window, &back_buffer);
}

void gfx_init() {
	root_window.fb = (uint32_t *) (uint32_t) multiboot_header->framebuffer;
	root_window.size.width = multiboot_header->framebuffer_width;
	root_window.size.height = multiboot_header->framebuffer_height;

	back_buffer.fb = malloc((root_window.size.width * root_window.size.height) * 4);
	back_buffer.size.width = root_window.size.width;
	back_buffer.size.height = root_window.size.height;

	background.fb = malloc((root_window.size.width * (root_window.size.height - taskbar_height)) * 4);
	background.cursor.x = 0;
	background.cursor.y = 0;
	background.x = 0;
	background.y = 0;
	background.size.width = root_window.size.width;
	background.size.height = root_window.size.height - taskbar_height;
	memset32(background.fb, background_colour, background.size.width * background.size.height);

	cursor.fb = malloc(1024);
	cursor.x = multiboot_header->framebuffer_width / 2;
	cursor.y = multiboot_header->framebuffer_height / 2;
	cursor.size.width = 16;
	cursor.size.height = 16;
	gfx_char_draw(u'\ue01e', 0, 0, 0xff000000, &cursor);

	taskbar.fb = malloc((root_window.size.width * taskbar_height) * 4);
	taskbar.cursor.x = 0;
	taskbar.cursor.y = 0;
	taskbar.x = 0;
	taskbar.y = root_window.size.height - taskbar_height;
	taskbar.size.width = root_window.size.width;
	taskbar.size.height = taskbar_height;

	startbutton.text = u"'Εναρζη";
	startbutton.icon = start_icon;
	testbutton.text = u"Тест";
	testbutton.icon = test_icon;
	taskbar_buttons = linked_add(taskbar_buttons, &startbutton);
	taskbar_buttons = linked_add(taskbar_buttons, &testbutton);
	taskbar_updated = 1;
	gfx_init_done = 1;
}
