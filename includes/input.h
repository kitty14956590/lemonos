#pragma once

#include <stdint.h>

typedef struct {
    int type;
    // thats it !
} event_t;

typedef void (* input_callback_t)(event_t * event);

#include <input.h>
#include <multitasking.h>

typedef struct {
	int sysrq; // is sysrq held
    int lctrl; // is left control held?
    int rctrl; // is right control held?
    int super; // is super held
    int meta; // is meta (ralt) held
    int lalt; // is left alt you get it
    int ralt;
    int lshift;
    int rshift;

    int caps; // is caps lock on
    int metalock; // is meta lock on
} keyboard_held_t;

typedef struct {
	int left;
	int right;
	int middle;
} mouse_held_t;

typedef struct {
    int type;
    uint32_t keycode;
    int pressed; // is this pressed or nah
    // you can impliment the keys held shit yourself too if you want
    keyboard_held_t * held;
} kbd_event_t;

typedef struct {
    int type;
    int x; // where mouse (x, y)
    int y;
    int delta_x; // raw movement straight of the mouse (can go past screen bounds)
    int delta_y;
    int bdelta_x; // mouse movement computed by whoever sent you this event (bound to screen)
    int bdelta_y;
	mouse_held_t * held;
} mouse_event_t;

typedef struct {
    int event;
    uint64_t pid;
    void * data;
} ipc_event_t;

enum {
    EVENT_KEYBOARD,
    EVENT_MOUSE,
    EVENT_IPC, // for processes to talk (maybe a bad idea ?)
};

uint16_t event_to_char(kbd_event_t * event, int force_layer);
uint16_t event_to_ascii_char(kbd_event_t * event);
void broadcast_event(event_t * event);
void send_event(event_t * event, process_t * process);
void dump_event(event_t * event);