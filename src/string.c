#include <stdint.h>

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
