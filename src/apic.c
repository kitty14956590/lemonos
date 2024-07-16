#include <acpi.h>
#include <apic.h>

unsigned char * apic_address = 0;
unsigned char * io_apic_address = 0;
APIC_t * apics[32];
IOApic_t * IOApic;

// named like this cause its in apic.c and its enable_apic
void apic_enable_apic() {
	*((uint32_t *) (apic_address + 0x0080)) = 0;
	*((uint32_t *) (apic_address + 0x00e0)) = 0xffffffff;
	*((uint32_t *) (apic_address + 0x00d0)) = 0x01000000;
	*((uint32_t *) (apic_address + 0x00f0)) = 0x1ff;
}
void apic_disable_ioapic() {
	*((uint32_t *) (io_apic_address)) = 1;
	uint32_t count = (((*((uint32_t *) (io_apic_address + 0x10))) >> 16) & 0xff) + 1;
	for (uint32_t i = 0; i < count; i++) {
		*((uint32_t *) (io_apic_address)) = 1;
		*((uint32_t *) (io_apic_address + (0x10 + (i * 2)))) = 0x10000;
		*((uint32_t *) (io_apic_address + (0x10 + (i * 2) + 1))) = 0x10000;
	}
}

void apic_init() {
	apic_address = (unsigned char *) MADT->ApicAddr;
	uint8_t * p = (uint8_t *) (MADT + 1);
	uint8_t * end = (uint8_t *) MADT + MADT->header.length;
	while (p < end) {
		APICHeader_t * header = (APICHeader_t *) p;
		APIC_t * apic = (APIC_t *) p;
		switch (header->type) {
			case APIC_LOCAL:
				if (cpu_count < 32) {
				apics[cpu_count] = apic;
				}
				cpu_count++;
			case APIC_IO:
				IOApic = (IOApic_t *) apic;
				io_apic_address = (unsigned char *) IOApic->address;
		}
		p += header->length;
	}
	apic_enable_apic();
	apic_disable_ioapic();
}