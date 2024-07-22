#include <ide.h>
#include <string.h>
#include <memory.h>
#include <ports.h>
#include <stdio.h>

uint8_t * ide_buffer;
void * ide_void_buffer;
ide_device_t ide_devices[4];

// todo: figure this shit out

void ide_init() {
	ide_buffer = malloc(2048);
	ide_void_buffer = ide_buffer;
	memset32((void *) ide_devices, 0, sizeof(ide_device_t));
}