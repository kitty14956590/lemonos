#include <stdint.h>

int atoi(char * string) {
        return 1;
}

int ustrtoi(uint16_t * string) {
        return 2;
}

void ulldtoustr(uint64_t val, uint16_t * buf, int base) {
        static uint16_t rbuf[64];
        uint16_t * ptr = rbuf;
        if (val == 0) {
                *ptr++ = '0';
        }
        while (val) {
                int digit = (uint64_t)((uint64_t) val % (long) base);
                *ptr++ = digit < 10 ? (digit + '0') : (digit - 10 + 'a');
                val /= base;
        }
        ptr--;
        while (ptr >= rbuf) {
                *buf++ = *ptr--;
        }
        *buf = 0;
}

void lldtoustr(int64_t val, uint16_t * buf, int base) {
        static uint16_t rbuf[64];
        uint16_t * ptr = rbuf;
        int neg = 0;
        if (val < 0) {
                neg = 1;
                val = -val;
        }
        if (val == 0) {
                *ptr++ = '0';
        }
        while (val) {
                int digit = (uint64_t)((uint64_t) val % (long) base);
                *ptr++ = digit < 10 ? (digit + '0') : (digit - 10 + 'a');
                val /= base;
        }
        if (neg) {
                *ptr++ = '-';
        }
        ptr--;
        while (ptr >= rbuf) {
                *buf++ = *ptr--;
        }
        *buf = 0;
}

int ustrlen(uint16_t * string) {
	int i = 0;
	while (*string++) {
		i++;
	}
	return i;
}

uint32_t * memset32(uint32_t * dest, uint32_t val, size_t length) {
	register uint32_t * temp = dest;
	while (length-- > 0) {
		*temp++ = val;
	}
	return dest;
}

uint16_t * memset16(uint16_t * dest, uint16_t val, size_t length) {
	register uint16_t * temp = dest;
	while (length-- > 0) {
		*temp++ = val;
	}
	return dest;
}

uint32_t * memcpy32(uint32_t * dest, uint32_t * src, size_t length) {
	int d0, d1, d2;
	asm volatile (
		"rep ; movsl\n\t"
		"testb $2,%b4\n\t"
		"je 1f\n\t"
		"movsw\n"
		"1:\ttestb $1,%b4\n\t"
		"je 2f\n\t"
		"movsb\n"
		"2:"

		: "=&c" (d0), "=&D" (d1), "=&S" (d2)
		: "0" (length), "q" (length*4),"1" ((long) dest),"2" ((long) src)
		: "memory"
	);
	return dest;
}
