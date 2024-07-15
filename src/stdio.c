#include <stdint.h>
#include <string.h>
#include <terminal.h>
#include <stdarg.h>

void printf(uint16_t * fmt, ...) {
    va_list listp;
    va_list * argv;
    uint16_t c;
    unsigned long ul = 0;
    long l = 0;
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
}

void cprintf(uint32_t colour, uint16_t * fmt, ...) {
    va_list listp;
    va_list * argv;
    uint16_t c;
    unsigned long ul = 0;
    long l = 0;
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
