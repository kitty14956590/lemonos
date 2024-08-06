#pragma once

extern int32_t mouse_y;
extern int32_t mouse_x;

void mouse_init();
void mouse_late_init();
void clip_mouse();

// 
enum {
	MOUSE,
	MOUSE_WITH_SCROLL = 0x03,
	MULTI_BUTTON_MOUSE,
};