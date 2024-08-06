#include <stdint.h>
#include <string.h>
#include <terminal.h>
#include <stdarg.h>
#include <serial.h>
#include <util.h>

void printf(uint16_t * fmt, ...) {
	disable_interrupts();
	va_list listp;
	va_list * argv;
	uint16_t c;
	unsigned long ul = 0;
	long l = 0;
	double f = 0;
	uint16_t * p;
	uint16_t buffer[128];
	unsigned char * buffer8;
	va_start(listp, fmt);
	argv = &listp;
	memset16(buffer, 0, 128);
	while ((c = *fmt) != u'\0') {
		if (c != u'%') {
			terminal_putc(c);
			fmt++;
			continue;
		} else {
			fmt++;
			c = *fmt;
		if (c == u'0') {
			break;
		}
		switch (c) {
			default:
				break;
			case u'x':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 16);
				terminal_print(buffer);
				break;
			case u'b':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 2);
				terminal_print(u"0b");
				terminal_print(buffer);
				break;
			case u'r':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 16);
				ul = ustrlen(buffer);
				ul = 8 - ul;
				terminal_print(u"0x");
				while (ul--) {
					terminal_print(u"0");
				}
				terminal_print(buffer);
				break;
			case u'f':
				f = (double) va_arg(*argv, double);
				ftoustr(f, buffer, 10);
				terminal_print(buffer);
				break;
			case u'd':
				l = (int32_t) va_arg(*argv, long);
				lldtoustr(l, buffer, 10);
				terminal_print(buffer);
				break;
			case u'o':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 8);
				terminal_print(buffer);
				break;
			case u'u':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 10);
				terminal_print(buffer);
				break;
			case u's':
				p = (uint16_t *) va_arg(*argv, uint16_t *);
				terminal_print(p);
				break;
			case u'c':
				ul = (uint16_t) va_arg(*argv, uint16_t);
				terminal_putc(ul);
				break;
			case u'%':
				terminal_putc(u'%');
				break;
		}
		fmt++;
		continue;
		}
	}
	va_end(listp);
	enable_interrupts();
}

void cprintf(uint32_t colour, uint16_t * fmt, ...) {
	disable_interrupts();
	va_list listp;
	va_list * argv;
	uint16_t c;
	unsigned long ul = 0;
	long l = 0;
	double f = 0;
	uint16_t * p;
	uint16_t buffer[128];
	unsigned char * buffer8;
	va_start(listp, fmt);
	argv = &listp;
	memset16(buffer, 0, 128);
	while ((c = *fmt) != u'\0') {
		if (c != u'%') {
			terminal_cputc(c, colour);
			fmt++;
			continue;
		} else {
			fmt++;
			c = *fmt;
		if (c == u'0') {
			break;
		}
		switch (c) {
			default:
				break;
			case u'x':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 16);
				terminal_cprint(buffer, colour);
				break;
			case u'r':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 16);
				ul = ustrlen(buffer);
				ul = 8 - ul;
				terminal_cprint(u"0x", colour);
				while (ul--) {
					terminal_cprint(u"0", colour);
				}
				terminal_cprint(buffer, colour);
				break;
			case u'b':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 2);
				terminal_cprint(u"0b", colour);
				terminal_cprint(buffer, colour);
				break;
			case u'f':
				f = (double) va_arg(*argv, double);
				ftoustr(f, buffer, 10);
				terminal_cprint(buffer, colour);
				break;
			case u'd':
				l = (int32_t) va_arg(*argv, long);
				lldtoustr(l, buffer, 10);
				terminal_cprint(buffer, colour);
				break;
			case u'o':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 8);
				terminal_cprint(buffer, colour);
				break;
			case u'u':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 10);
				terminal_cprint(buffer, colour);
				break;
			case u's':
				p = (uint16_t *) va_arg(*argv, uint16_t *);
				terminal_cprint(p, colour);
				break;
			case u'c':
				ul = (uint16_t) va_arg(*argv, uint16_t);
				terminal_cputc(ul, colour);
				break;
			case u'%':
				terminal_cputc(u'%', colour);
				break;
		}
		fmt++;
		continue;
		}
	}
	va_end(listp);
	enable_interrupts();
}

void iprintf(uint32_t colour, uint16_t * fmt, ...) {
	va_list listp;
	va_list * argv;
	uint16_t c;
	unsigned long ul = 0;
	long l = 0;
	double f = 0;
	uint16_t * p;
	uint16_t buffer[128];
	unsigned char * buffer8;
	va_start(listp, fmt);
	argv = &listp;
	memset16(buffer, 0, 128);
	while ((c = *fmt) != u'\0') {
		if (c != u'%') {
			terminal_cputc(c, colour);
			fmt++;
			continue;
		} else {
			fmt++;
			c = *fmt;
		if (c == u'0') {
			break;
		}
		switch (c) {
			default:
				break;
			case u'x':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 16);
				terminal_cprint(buffer, colour);
				break;
			case u'r':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 16);
				ul = ustrlen(buffer);
				ul = 8 - ul;
				terminal_cprint(u"0x", colour);
				while (ul--) {
					terminal_cprint(u"0", colour);
				}
				terminal_cprint(buffer, colour);
				break;
			case u'b':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 2);
				terminal_cprint(u"0b", colour);
				terminal_cprint(buffer, colour);
				break;
			case u'f':
				f = (double) va_arg(*argv, double);
				ftoustr(f, buffer, 10);
				terminal_cprint(buffer, colour);
				break;
			case u'd':
				l = (int32_t) va_arg(*argv, long);
				lldtoustr(l, buffer, 10);
				terminal_cprint(buffer, colour);
				break;
			case u'o':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 8);
				terminal_cprint(buffer, colour);
				break;
			case u'u':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 10);
				terminal_cprint(buffer, colour);
				break;
			case u's':
				p = (uint16_t *) va_arg(*argv, uint16_t *);
				terminal_cprint(p, colour);
				break;
			case u'c':
				ul = (uint16_t) va_arg(*argv, uint16_t);
				terminal_cputc(ul, colour);
				break;
			case u'%':
				terminal_cputc(u'%', colour);
				break;
		}
		fmt++;
		continue;
		}
	}
	va_end(listp);
}

// log printf
void lprintf(uint16_t * fmt, ...) {
	disable_interrupts();
	va_list listp;
	va_list * argv;
	uint16_t c;
	uint64_t ll = 0;
	int64_t ull = 0;
	unsigned long ul = 0;
	long l = 0;
	double f = 0;
	uint16_t * p;
	uint16_t buffer[128];
	unsigned char * buffer8;
	va_start(listp, fmt);
	argv = &listp;
	memset16(buffer, 0, 128);
	while ((c = *fmt) != u'\0') {
		if (c != u'%') {
			serial_outw(COM1_PORT, c);
			fmt++;
			continue;
		} else {
			fmt++;
			c = *fmt;
		if (c == u'0') {
			break;
		}
		switch (c) {
			default:
				break;
			case u'x':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 16);
				serial_outs(COM1_PORT, buffer);
				break;
			case u'b':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 2);
				serial_outs(COM1_PORT, u"0b");
				serial_outs(COM1_PORT, buffer);
				break;
			case u'r':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 16);
				ul = ustrlen(buffer);
				ul = 8 - ul;
				serial_outs(COM1_PORT, u"0x");
				while (ul--) {
					serial_outs(COM1_PORT, u"0");
				}
				serial_outs(COM1_PORT, buffer);
				break;
			case u'f':
				f = (double) va_arg(*argv, double);
				ftoustr(f, buffer, 10);
				serial_outs(COM1_PORT, buffer);
				break;
			case u'd':
				l = (int32_t) va_arg(*argv, long);
				lldtoustr(l, buffer, 10);
				serial_outs(COM1_PORT, buffer);
				break;
			case u'l':
				ull = (uint64_t) va_arg(*argv, unsigned long long);
				ulldtoustr(ull, buffer, 10);
				serial_outs(COM1_PORT, buffer);
				break;
			case u'o':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 8);
				serial_outs(COM1_PORT, buffer);
				break;
			case u'u':
				ul = (uint32_t) va_arg(*argv, unsigned long);
				ulldtoustr(ul, buffer, 10);
				serial_outs(COM1_PORT, buffer);
				break;
			case u's':
				p = (uint16_t *) va_arg(*argv, uint16_t *);
				serial_outs(COM1_PORT, p);
				break;
			case u'c':
				ul = (uint16_t) va_arg(*argv, uint16_t);
				serial_outw(COM1_PORT, ul);
				break;
			case u'%':
				serial_outw(COM1_PORT, u'%');
				break;
		}
		fmt++;
		continue;
		}
	}
	va_end(listp);
	enable_interrupts();
}