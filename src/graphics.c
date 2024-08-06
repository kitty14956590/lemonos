#include <graphics.h>
#include <input.h>
#include <multitasking.h>
#include <multiboot.h>
#include <memory.h>
#include <font.h>
#include <string.h>
#include <linked.h>
#include <stdio.h>
#include <mouse.h>

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
rect_2d_t contextmenu;
rect_2d_t startmenu;
rect_2d_t window_buffer;

window_t fake_root_window; // fake root_window window

window_t * active_window;

linked_t * taskbar_buttons;
linked_t * windows;
uint32_t button_offset;
uint32_t button_border = 0xff000000;
int taskbar_height = 28;
int taskbar_updated = 0;
int taskbar_button_count = 0;
int window_count = 0;

uint32_t background_colour = 0xff008080; // 0xff5500aa;
uint32_t taskbar_colour = 0xffaaaaaa;
uint32_t taskbar_highlight_colour = 0xffffffff;
uint32_t tertiary_colour = 0xff000080;
uint32_t window_background = 0xffaaaaaa;

int gfx_init_done = 0;

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
		dest = rect2->fb + (((rect->y + i) * rect2->size.width) + rect->x);
		src = rect->fb + (i * rect->size.width);
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
void gfx_ndraw_rect(rect_2d_t * rect2, int width, int height, int x, int y, uint32_t colour) {
	uint32_t * dest;
	for (int i = 0; i < height; i++) {
		if ((i + y) < 0) {
			continue;
		}
		if ((i + y) >= rect2->size.height) {
			break;
		}
		dest = rect2->fb + (((y + i) * rect2->size.width) + x);
		for (int b = 0; b < width; b++) {
			if ((b + x) < 0) {
				continue;
			}
			if ((b + x) >= rect2->size.width) {
				break;
			}
			*(dest + b) = colour;
		}
	}
	return;
}

void rect_2d_memset32(rect_2d_t * rect, uint32_t colour, int length, int x, int y) {
	if (x > rect->size.width || y > rect->size.height) {
		return;
	}
	if (x + length > rect->size.width) {
		length = rect->size.width - x;
	}
	memset32(window_buffer.fb + (y * window_buffer.size.width) + x, colour, length);
}

uint32_t rgb_degrade(uint32_t colour, uint32_t target) {
	uint8_t r, g, b;
	uint8_t tr, tg, tb;
	r = (colour >> 16) & 0xff;
	g = (colour >> 8) & 0xff;
	b = colour & 0xff;
	tr = (target >> 16) & 0xff;
	tg = (target >> 8) & 0xff;
	tb = target & 0xff;
	if (r == tr && g == tg && b == tb) {
		return target | 0xff000000;
	}
	r = r == tr ? tr : (r > tr ? (r - 1) : (r + 1));
	g = g == tg ? tg : (g > tg ? (g - 1) : (g + 1));
	b = b == tb ? tb : (b > tb ? (b - 1) : (b + 1));
	return (r << 16) | (g << 8) | b | 0xff000000;
}

uint32_t draw_taskbar_button(taskbar_button_t * button, uint32_t x) {
	uint16_t * p = button->text;
	int i = 0;
	int len = *p == 0 ? 0 : ustrlen(p);
	int text_margin = *p == 0 ? 0 : TASKBAR_TEXT_MARGIN;
	rect_2d_t icon = {
		(TASKBAR_MARGIN * 2) + 1 + x + (i * TASKBAR_CHAR_SIZE), 8, {0, 0},
		button->icon,
		{16, 16}
	};

	rect_2d_adraw(&taskbar, &icon);

	while (i < len) {
		gfx_char_p_draw(*p++, 28 + x + (i * TASKBAR_CHAR_SIZE), 8, 0, &taskbar);
		i++;
	}

	button->x = TASKBAR_MARGIN + x;
	button->y = (TASKBAR_MARGIN * 2) + 1 + TASKBAR_ICON_SIZE + (len * TASKBAR_CHAR_SIZE) + text_margin + x;
	memset32(taskbar.fb + (root_window.size.width * 2) + TASKBAR_MARGIN + x + (TASKBAR_Y_START * taskbar.size.width), button_border, TASKBAR_ICON_SIZE + (len * TASKBAR_CHAR_SIZE) + text_margin + (TASKBAR_MARGIN * 2) + 2);
	memset32(taskbar.fb + (root_window.size.width * 2) + TASKBAR_MARGIN + x + (TASKBAR_Y_END * taskbar.size.width), button_border, TASKBAR_ICON_SIZE + (len * TASKBAR_CHAR_SIZE) + text_margin + (TASKBAR_MARGIN * 2) + 2);
	for (int i = TASKBAR_Y_END - 1; i > TASKBAR_Y_START; i--) {
		taskbar.fb[(root_window.size.width * 2) + TASKBAR_MARGIN + x + (i * taskbar.size.width)] = button_border;
		taskbar.fb[(root_window.size.width * 2) + TASKBAR_MARGIN + x + (TASKBAR_MARGIN * 2) + 1 + TASKBAR_ICON_SIZE + (len * TASKBAR_CHAR_SIZE) + text_margin + (i * taskbar.size.width)] = button_border;
	}
	return TASKBAR_ICON_SIZE + (len * TASKBAR_CHAR_SIZE) + (TASKBAR_MARGIN * 2) + 1 + text_margin + (TASKBAR_MARGIN * 2);
}

int draw_taskbar_callback(linked_t * node, void * p) {
	taskbar_button_t * button = node->p;
	button_offset += draw_taskbar_button(button, button_offset);
}

void draw_taskbar_buttons() {
	if (!taskbar_updated) {
		return;
	}
	button_offset = 0;
	memset32(taskbar.fb + (root_window.size.width * 2), taskbar_colour, root_window.size.width * taskbar_height);
	linked_iterate(taskbar_buttons, draw_taskbar_callback, 0);
	taskbar_updated = 0;
}

void gfx_draw_tertiary(window_t * window) {
	int length = 0;
	window->rect.y = window->y + TERTIARY_HEIGHT + (TERTIARY_MARGIN * 2); // top and bottom margin
	window->rect.x = window->x + 1 + WINDOW_PADDING; // left padding
	rect_2d_memset32(&window_buffer, 0xffffffff, window->rect.size.width + 2 + (WINDOW_PADDING * 2), window->x, window->y);
	rect_2d_memset32(&window_buffer, window_background, window->rect.size.width, window->x + 1 + WINDOW_PADDING, window->y + 1);
	rect_2d_memset32(&window_buffer, window_background, window->rect.size.width, window->x + 1 + WINDOW_PADDING, window->y + 2);
	rect_2d_memset32(&window_buffer, window_background, window->rect.size.width, window->x + 1 + WINDOW_PADDING, window->y + TERTIARY_HEIGHT + (TERTIARY_MARGIN * 2) + window->rect.size.height);
	rect_2d_memset32(&window_buffer, window_background, window->rect.size.width, window->x + 1 + WINDOW_PADDING, window->y + TERTIARY_HEIGHT + (TERTIARY_MARGIN * 2) + window->rect.size.height + 1);
	rect_2d_memset32(&window_buffer, 0xff000000, window->rect.size.width + 2 + (WINDOW_PADDING * 2), window->x, window->y + TERTIARY_HEIGHT + (TERTIARY_MARGIN * 2) + window->rect.size.height + WINDOW_PADDING);
	rect_2d_memset32(&window_buffer, window_background, window->rect.size.width, window->x + 1 + WINDOW_PADDING, window->y + TERTIARY_HEIGHT + 2);
	rect_2d_memset32(&window_buffer, window_background, window->rect.size.width, window->x + 1 + WINDOW_PADDING, window->y + TERTIARY_HEIGHT + 3);
	length = window->y + TERTIARY_HEIGHT + (TERTIARY_MARGIN * 2) + window->rect.size.height + WINDOW_PADDING;
	if (length > window_buffer.size.height) {
		length = window_buffer.size.height;
	}
	for (int i = window->y + 1; i < length; i++) {
		window_buffer.fb[(i * window_buffer.size.width) + window->x] = 0xffffffff;
		if ((window->x + 1 + window->rect.size.width + (WINDOW_PADDING * 2)) > window_buffer.size.width) {
			continue;
		}
		window_buffer.fb[(i * window_buffer.size.width) + window->x + 1 + window->rect.size.width + (WINDOW_PADDING * 2)] = 0xff000000;
	}
	length = window->y + TERTIARY_HEIGHT + (TERTIARY_MARGIN * 2) + window->rect.size.height + WINDOW_PADDING;
	if (length > window_buffer.size.height) {
		length = window_buffer.size.height;
	}
	for (int i = window->y + 1; i < length; i++) {
		window_buffer.fb[(i * window_buffer.size.width) + window->x + 1] = window_background;
		window_buffer.fb[(i * window_buffer.size.width) + window->x + 2] = window_background;
		if ((window->x + 1 + window->rect.size.width + WINDOW_PADDING) > window_buffer.size.width) {
			continue;
		}
		window_buffer.fb[(i * window_buffer.size.width) + window->x + 1 + window->rect.size.width + WINDOW_PADDING] = window_background;
		if ((window->x + 2 + window->rect.size.width + WINDOW_PADDING) > window_buffer.size.width) {
			continue;
		}
		window_buffer.fb[(i * window_buffer.size.width) + window->x + 2 + window->rect.size.width + WINDOW_PADDING] = window_background;
	}
	length = window->y + TERTIARY_HEIGHT + TERTIARY_MARGIN;
	for (int i = window->y + TERTIARY_MARGIN + 1; i < length; i++) {
		rect_2d_memset32(&window_buffer, tertiary_colour, window->rect.size.width, window->x + WINDOW_PADDING + 1, i);
	}
	uint16_t * p = window->text;
	int len = *p == 0 ? 0 : ustrlen(p);
	int i = 0;

	if (((len * 8) + window->x + 1 + WINDOW_PADDING + TERTIARY_TEXT_PADDING) > root_window.size.width) {
		len = (root_window.size.width - (window->x + 1 + WINDOW_PADDING + TERTIARY_TEXT_PADDING)) / 8;
	}

	while (i < len) {
		gfx_char_p_draw(*p++, window->x + 1 + WINDOW_PADDING + TERTIARY_TEXT_PADDING + (i * 8), window->y + TERTIARY_TEXT_PADDING + TERTIARY_MARGIN, 0xffffffff, &window_buffer);
		i++;
	}
}

void gfx_draw_window(window_t * window) {
	window->rect.y = window->y + TERTIARY_HEIGHT + (TERTIARY_MARGIN * 2); // top and bottom margin
	window->rect.x = window->x + 1 + WINDOW_PADDING; // left padding
	gfx_draw_tertiary(window);
	window->ondraw();
	rect_2d_afdraw(&window_buffer, &window->rect);
}

int gfx_draw_window_callback(linked_t * node, void * p) {
	window_t * window = node->p;
	gfx_draw_window(window);
}

void draw_windows() {
	linked_iterate(windows, gfx_draw_window_callback, 0);
}

void draw_frame() {
	if (!gfx_init_done) {
		return;
	}
	draw_taskbar_buttons();
	draw_windows();
	rect_2d_draw(&back_buffer, &background);
	rect_2d_afdraw(&back_buffer, &window_buffer);
	rect_2d_draw(&back_buffer, &taskbar);
	rect_2d_afdraw(&back_buffer, &cursor);
	rect_2d_draw(&root_window, &back_buffer);
}

int gfx_startbutton_clicked() {
	cprintf(7, u"Start button clicked\n");
}

int gfx_find_taskbar_callback(linked_t * node, void * p) {
	taskbar_button_t ** button = (taskbar_button_t **) p;
	if (((size_t) node->p) == ((size_t) *button)) {
		return 1;
	}
	return 0;
}

linked_t * gfx_find_taskbar(taskbar_button_t * button) {
	return linked_find(taskbar_buttons, gfx_find_taskbar_callback, &button);
}

int gfx_default_callback() {}
void gfx_default_event_handler(event_t * event) {}

taskbar_button_t * gfx_create_taskbar(uint16_t * text, gfx_callback_t onclick) {
	taskbar_button_t * button = malloc(sizeof(taskbar_button_t) + 8);
	button->text = ustrdup(text);
	button->icon = malloc(1024);
	button->click = onclick;
	taskbar_buttons = linked_add(taskbar_buttons, button);
	taskbar_button_count++;
	return button;
}

void gfx_move_window(window_t * window, int x, int y) {
	int width = root_window.size.width;
	int height = root_window.size.height - taskbar.size.height;
	int john = (x < 0) ? 0 : (x > width - 8) ? width - 8 : x;
	int steve = (y < 0) ? 0 : (y > height - (WINDOW_PADDING * 2) - TERTIARY_HEIGHT) ? height - (WINDOW_PADDING * 2) - TERTIARY_HEIGHT : y;
	if (window->x == john && window->y == steve) {
		return;
	}
	gfx_ndraw_rect(&window_buffer, window->rect.size.width + (WINDOW_PADDING * 2) + 2, window->rect.size.height + TERTIARY_HEIGHT + (WINDOW_PADDING * 2) + TERTIARY_MARGIN + 1, window->x, window->y, 0);
	window->x = john;
	window->y = steve;
}

window_t * gfx_create_window(uint16_t * text, int width, int height) {
	window_t * window = malloc(sizeof(window_t) + 8);
	taskbar_button_t * taskbar = gfx_create_taskbar(text, gfx_default_callback);
	memcpy(taskbar->icon, error_icon, 1024);
	window->x = root_window.size.width / 2;
	window->y = root_window.size.height / 2;
	window->rect.size.width = width;
	window->rect.size.height = height;
	window->rect.fb = malloc((width * height) * 4);
	memset32(window->rect.fb, 0xff808080, width * height);
	window->text = ustrdup(text);
	window->id = window_count;
	window->shown = 1;
	window->onopen = gfx_default_callback;
	window->onfocus = gfx_default_callback;
	window->onclose = gfx_default_callback;
	window->ondraw = gfx_default_callback;
	window->onopen = gfx_default_callback;
	window->send_event = gfx_default_event_handler;
	// window->taskbar = gfx_find_taskbar(taskbar);
	windows = linked_add(windows, window);
	window_count++;
	return window;
}

void gfx_cleanup_window(window_t * window) {
	taskbar_button_t * button = window->taskbar->p;
	free(button->text);
	free(button->icon);
	free(button);
	linked_delete(window->taskbar);
	free(window);
}

int gfx_find_button(linked_t * node, void * pass) {
	mouse_event_t * event = pass;
	taskbar_button_t * button = node->p;
	return (event->x >= button->x) && (event->x <= button->y);
}

int gfx_find_window(linked_t * node, void * pass) {
	mouse_event_t * event = pass;
	window_t * window = node->p;
	int width = window->rect.size.width + (WINDOW_PADDING * 2);
	int height = window->rect.size.height + TERTIARY_HEIGHT + (TERTIARY_MARGIN * 2) + WINDOW_PADDING;
	return (event->y > window->y) && (event->y < (window->y + height)) && (event->x > window->x) && (event->x < (window->x + width));
}

window_t * gfx_mouse_touching(mouse_event_t * event) {
	linked_t * node;
	node = linked_find_back(windows, gfx_find_window, event);
	if (!node) {
		return (void *) 0;
	}
	return (window_t *) node->p;
}

void gfx_taskbar_event(event_t * event) {
	if (event->type != EVENT_MOUSE) {
		return;
	}
	mouse_event_t * mouseevent = (mouse_event_t *) event;
	linked_t * node;
	taskbar_button_t * button;
	mouse_held_t * held = mouseevent->held;
	static int click = 0;
	if (held->left == click || held->left == 0) {
		click = held->left;
		return;
	}
	click = held->left;
	if (mouseevent->y < (taskbar.y + 3) || mouseevent->x < 3 || mouseevent->y > root_window.size.height - 3) {
		return;
	}
	node = linked_find(taskbar_buttons, gfx_find_button, mouseevent);
	if (!node) {
		return;
	}
	button = node->p;
	button->click();
}

void gfx_event_window_tertiary(mouse_event_t * event, window_t * window) {
	mouse_event_t * mouseevent = (mouse_event_t *) event;
	mouse_held_t * held = mouseevent->held;
	static int click = 0;
	static int holding = 0;
	if (held->left != click) {
		click = held->left;
		holding = ((mouseevent->y > window->y + 1) && (mouseevent->x > window->x + 1) && (mouseevent->y < window->y + 3 + TERTIARY_HEIGHT) && (mouseevent->x < window->x + (WINDOW_PADDING * 2) + window->rect.size.width));
	}
	if (!window || !holding) {
		return;
	}
	gfx_move_window(window, window->x + mouseevent->bdelta_x, window->y - mouseevent->bdelta_y);
}

void gfx_event_window_inside(mouse_event_t * event, window_t * window) {
	mouse_event_t * copy;
	int height = TERTIARY_HEIGHT + WINDOW_PADDING + TERTIARY_MARGIN; // tertiary hight plus margin
	int ax = event->x - window->x - WINDOW_PADDING; // adjusted x and y
	int ay = event->y - window->y - height;
	if ((ax < 0) || (ay < 0) || (ax > window->rect.size.width) || (ay > window->rect.size.height)) {
		return; // outside of window context
	}
	copy = malloc(sizeof(mouse_event_t) + 8);
	memcpy(copy, event, sizeof(mouse_event_t));
	copy->x = ax;
	copy->y = ay; // obbvious
	copy->bdelta_x = window->x - copy->x;
	copy->bdelta_y = window->y - height - event->y;
	window->send_event((event_t *) copy);
	free(copy);
}

void gfx_window_event(event_t * event) {
	if (event->type != EVENT_MOUSE) {
		return;
	}
	mouse_event_t * mouseevent = (mouse_event_t *) event;
	mouse_held_t * held = mouseevent->held;
	static window_t * window = (window_t *) 0;
	static int holding = 0;
	static int click = 0;
	if (held->left != click) {
		click = held->left;
		if (click == 0 && holding) {
			window = (window_t *) 0;
			holding = 0;
			return;
		}
		window = gfx_mouse_touching(mouseevent);
		if (!window) {
			return;
		}
		holding = mouseevent->y < window->y + 3 + TERTIARY_HEIGHT;
	}
	click = held->left;
	if (holding) {
		gfx_event_window_tertiary(mouseevent, window);
		return;
	}
	window = gfx_mouse_touching(mouseevent);
	if (!window) {
		return;
	}
	gfx_event_window_inside(mouseevent, window);
}

void gfx_handle_event(event_t * event) {
	if (mouse_y > taskbar.y) {
		gfx_taskbar_event(event);
	} else {
		gfx_window_event(event);
	}
	return;
}

window_t * test_window;
rect_2d_t test_rect;

void gfx_window_handle_event(event_t * event) {
	mouse_event_t * mouseevent = (mouse_event_t *) event;
	test_rect.x = mouseevent->x - 4;
	test_rect.y = mouseevent->y - 4;
	if (!mouseevent->held->right && !mouseevent->held->left) {
		memset32(test_rect.fb, 0xffffffff, 9 * 9);
		return;
	}
	memset32(test_rect.fb, 0xff000000 | (mouseevent->held->right * 0xff) | ((mouseevent->held->left * 0xff) << 16), 9 * 9);
}

int gfx_window_ondraw() {
	memset32(test_window->rect.fb, 0xff808080, 200 * 100);
	rect_2d_draw(&test_window->rect, &test_rect);
}

void gfx_init() {
	root_window.fb = (uint32_t *) (uint32_t) multiboot_header->framebuffer;
	root_window.size.width = multiboot_header->framebuffer_width;
	root_window.size.height = multiboot_header->framebuffer_height;
	memset32(root_window.fb, 0x00000000, root_window.size.width * root_window.size.height);

	back_buffer.fb = malloc((root_window.size.width * root_window.size.height) * 4);
	back_buffer.size.width = root_window.size.width;
	back_buffer.size.height = root_window.size.height;
	memset32(back_buffer.fb, 0x00000000, root_window.size.width * root_window.size.height);

	window_buffer.fb = malloc((root_window.size.width * root_window.size.height - taskbar_height - 2) * 4);
	window_buffer.size.width = root_window.size.width;
	window_buffer.size.height = root_window.size.height - taskbar_height - 2;
	window_buffer.x = 0;
	window_buffer.y = 0;
	memset32(window_buffer.fb, 0x00000000, window_buffer.size.width * window_buffer.size.height);

	background.fb = malloc((root_window.size.width * (root_window.size.height - taskbar_height - 2)) * 4);
	background.cursor.x = 0;
	background.cursor.y = 0;
	background.x = 0;
	background.y = 0;
	background.size.width = root_window.size.width;
	background.size.height = root_window.size.height - taskbar_height - 2;
	memset32(background.fb, background_colour, background.size.width * background.size.height);

	cursor.fb = malloc(1024);
	cursor.x = multiboot_header->framebuffer_width / 2;
	cursor.y = multiboot_header->framebuffer_height / 2;
	cursor.size.width = 16;
	cursor.size.height = 16;
	memset(cursor.fb, 0, 1024);
	gfx_char_draw(u'\ue01e', 0, 0, 0xff000000, &cursor);

	taskbar.fb = malloc((root_window.size.width * (taskbar_height + 2)) * 4);
	taskbar.cursor.x = 0;
	taskbar.cursor.y = 0;
	taskbar.x = 0;
	taskbar.y = (root_window.size.height - taskbar_height) - 2;
	taskbar.size.width = root_window.size.width;
	taskbar.size.height = taskbar_height + 2;
	memset32(taskbar.fb, taskbar_highlight_colour, root_window.size.width * 2);
	memcpy(gfx_create_taskbar(u"Start", gfx_startbutton_clicked)->icon, start_icon, 1024);

	test_rect.size.width = 9;
	test_rect.size.height = 9;
	test_rect.x = 0;
	test_rect.y = 0;
	test_rect.fb = malloc((9 * 9) * 4);
	memset32(test_rect.fb, 0xffffffff, 9 * 9);
	test_window = gfx_create_window(u"Test", 200, 100);
	gfx_move_window(test_window, 100, 60);
	test_window->send_event = gfx_window_handle_event;
	test_window->ondraw = gfx_window_ondraw;

	fake_root_window = (window_t) {root_window, ustrdup(u"root"), 0, 0, 0, 0, 1};
	fake_root_window.onopen = gfx_default_callback;
	fake_root_window.onfocus = gfx_default_callback;
	fake_root_window.onclose = gfx_default_callback;
	fake_root_window.ondraw = gfx_default_callback;
	fake_root_window.onopen = gfx_default_callback;
	fake_root_window.send_event = gfx_default_event_handler;
	fake_root_window.taskbar = taskbar_buttons;
	active_window = &fake_root_window;

	taskbar_updated = 1;
	gfx_init_done = 1;
}

void gfx_late_init() {
	create_event_zombie(u"videod", gfx_handle_event);
}
