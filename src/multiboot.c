#include <multiboot.h>

multiboot_header_t * multiboot_header;

int parse_multiboot(uint32_t eax, uint32_t ebx) {
    multiboot_header = (multiboot_header_t *) ebx;
    return eax == 0x2BADB002;
}
