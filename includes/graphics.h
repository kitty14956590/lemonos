#pragma once

#include <stdint.h>
#include <linked.h>
#include <math.h>
#include <input.h>

enum CHARACTER_TYPES {
	FONT_LEGACY,
	FONT_BLANK,
	FONT_COMBINING,
	FONT_TRUECOLOUR = 16,
};

extern int legacy_colour[16];

typedef void (* font_drawer_t)(uint32_t * fb, uint32_t chr, uint32_t colour, uint32_t position);
typedef int (* gfx_callback_t)();

typedef struct size_2d {
	int width;
	int height;
} size_2d_t;

typedef struct _rect_2d {
	int x;
	int y;
	vect_2d_t cursor;
	uint32_t * fb;
	size_2d_t size;
} rect_2d_t;

typedef struct {
	uint32_t * icon;
	uint16_t * text;
	gfx_callback_t click;
	// misnomer
	int x;
	int y;
} taskbar_button_t;

typedef struct _window {
	rect_2d_t rect;
	uint16_t * text;
	int id;
	int x;
	int y;
	int z; // 3d !!!!!!!
	int shown; // is shown?
	gfx_callback_t onopen;
	gfx_callback_t onfocus;
	gfx_callback_t onclose;
	gfx_callback_t ondraw;
	input_callback_t send_event;
	linked_t * taskbar;
} window_t;

extern rect_2d_t root_window;
extern rect_2d_t back_buffer;
extern rect_2d_t background;
extern rect_2d_t taskbar;
extern rect_2d_t cursor;
extern window_t * active_window;
extern linked_t * taskbar_buttons;
extern uint32_t button_offset;
extern int taskbar_height;
extern int fps;
extern int gfx_init_done;

void gfx_init();
void gfx_late_init();
int gfx_string_draw(uint16_t * string, int x, int y, int p, uint32_t colour, rect_2d_t * rect);
int txt_string_draw(uint16_t * string, int x, int y, uint32_t colour, rect_2d_t * rect);
uint32_t rgb_degrade(uint32_t colour, uint32_t target);

void gfx_cleanup_window(window_t * window);

uint32_t draw_taskbar_button(taskbar_button_t * button, uint32_t x);

window_t * create_window(rect_2d_t rect, int tertiary, gfx_callback_t close);
int open_window(window_t * window);
int close_window(window_t * window);
void draw_frame();

#define TASKBAR_MARGIN 3

// icon is sqaure (16x16)
#define TASKBAR_ICON_SIZE 16
#define TASKBAR_CHAR_SIZE 8

// position of top and bottom of tasbar button
#define TASKBAR_Y_START 3
#define TASKBAR_Y_END 24

// margin between icon and text
#define TASKBAR_TEXT_MARGIN 6

#define TERTIARY_HEIGHT 19
#define TERTIARY_MARGIN 2
#define TERTIARY_TEXT_PADDING 2
#define WINDOW_PADDING 2