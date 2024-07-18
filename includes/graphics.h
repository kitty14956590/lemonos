#pragma once

#include <stdint.h>
#include <linked.h>
#include <math.h>

enum CHARACTER_TYPES {
	FONT_LEGACY,
	FONT_BLANK,
	FONT_COMBINING,
	FONT_TRUECOLOUR = 16,
};

extern int legacy_colour[16];

typedef void (* font_drawer_t)(uint32_t * fb, uint32_t chr, uint32_t colour, uint32_t position);
typedef void (* gfx_callback_t)();

typedef struct size_2d {
	int width;
	int height;
} size_2d_t;

typedef struct _rect_2d {
	int x;
	int y;
	int id;
	vect_2d_t cursor;
	uint32_t * fb;
	size_2d_t size;
} rect_2d_t;

typedef struct _window {
	rect_2d_t window;
	int tertiary;
	gfx_callback_t close;
	linked_t * taskbar;
} window_t;

typedef struct {
	uint32_t * icon;
	uint16_t * text;
	gfx_callback_t click;
} taskbar_button_t;

extern rect_2d_t root_window;
extern rect_2d_t taskbar;
extern rect_2d_t cursor;
extern linked_t * taskbar_buttons;
extern taskbar_button_t startbutton;
extern uint32_t button_offset;
extern int taskbar_height;

void gfx_init();
int txt_string_draw(uint16_t * string, int x, int y, uint32_t colour, rect_2d_t * rect);

uint32_t draw_taskbar_button(taskbar_button_t * button, uint32_t x);

window_t * create_window(rect_2d_t rect, int tertiary, gfx_callback_t close);
int open_window(window_t * window);
int close_window(window_t * window);
void draw_frame();