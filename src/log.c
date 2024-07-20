#include <ports.h>
#include <parallel.h>
#include <serial.h>
#include <stdint.h>

void info_log(uint16_t * name, uint16_t * message) {
	parallel_outs(u"[ * ] ");
	parallel_outs(name);
	parallel_outs(u" - ");
	parallel_outs(message);
	parallel_outs(u"\r\n");
	if (com2_works) {
		serial_outs(COM2_PORT, u"[ * ] ");
		serial_outs(COM2_PORT, name);
		serial_outs(COM2_PORT, u" - ");
		serial_outs(COM2_PORT, message);
		serial_outs(COM2_PORT, u"\r\n");
	}
}

void error_log(uint16_t * name, uint16_t * message) {
	parallel_outs(u"[ ! ] ");
	parallel_outs(name);
	parallel_outs(u" - ");
	parallel_outs(message);
	parallel_outs(u"\r\n");
	if (com2_works) {
		serial_outs(COM2_PORT, u"[ ! ] ");
		serial_outs(COM2_PORT, name);
		serial_outs(COM2_PORT, u" - ");
		serial_outs(COM2_PORT, message);
		serial_outs(COM2_PORT, u"\r\n");
	}
}