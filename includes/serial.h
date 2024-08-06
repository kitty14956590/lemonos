#pragma once

#define COM1_PORT 0x3f8
#define COM2_PORT 0x2f8

extern int com1_works;
extern int com2_works;

void serial_init();
void serial_outb(int port, unsigned char data);
void serial_outw(int port, uint16_t data);
void serial_outs(int port, uint16_t * data);
void serial_outbin(int port, void * data, size_t length);