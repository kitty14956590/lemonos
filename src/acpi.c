#include <stdint.h>
#include <acpi.h>
#include <ports.h>
#include <string.h>

RSDP_t * RSDP = 0;
FADT_t * FADT = 0;
MADT_t * MADT = 0;
SBST_t * SBST = 0;
HPET_t * HPET = 0;
ACPISDTHeader_t * RSDT = 0;

uint32_t cpu_count = 0;

int acpi_working = 0;

void acpi_parse_dt(ACPISDTHeader_t * table) {
		if (memcmp(table->signature, "FACP", 4) == 0) {
				FADT = (FADT_t *) table;
		} else if (memcmp(table->signature, "APIC", 4) == 0) {
				MADT = (MADT_t *) table;
		} else if (memcmp(table->signature, "SBST", 4) == 0) {
				SBST = (SBST_t *) table;
		} else if (memcmp(table->signature, "HPET", 4) == 0) {
				HPET = (HPET_t *) table;
		}
}

void acpi_find() {
	unsigned char * mem = (unsigned char *) 0x000E0000;
	while ((uint32_t) mem < 0x000FFFFF) {
		if (mem[0] == 'R' && mem[1] == 'S' && mem[2] == 'D' && mem[3] == ' ' && mem[4] == 'P' && mem[5] == 'T' && mem[6] == 'R' && mem[7] == ' ') {
			break;
		}
		mem += 16;
	}
	if ((uint32_t) mem != 0x100000) {
		RSDP = (RSDP_t *) mem;
		RSDT = (ACPISDTHeader_t *) RSDP->address;
		uint32_t * SDTP = (uint32_t *) (RSDT + 1);
		uint32_t * end = (uint32_t *) ((uint8_t *) RSDT + RSDT->length);
		while (SDTP < end) {
			uint32_t addr = *SDTP++;
			acpi_parse_dt((ACPISDTHeader_t *) addr);
		}
	}
}

int acpi_reboot() {
	if (!FADT) {
		return 0;
	}
	switch (FADT->ResetReg.AddressSpace) {
		default:
			return 0;

		case GAS_PIO:
			outb(FADT->ResetReg.Address, FADT->ResetValue);
			break;

		case GAS_MEMORY_MAPPED:
			*((uint8_t *) ((uint32_t) FADT->ResetReg.Address)) = FADT->ResetValue;
			break;

		case GAS_PCI_CONFIG:
			// unreadable
			break;
	}
	return 1;
}

int acpi_shutdown() {
	if (!FADT) {
		return 0;
	}
	outw((unsigned int) FADT->PM1aControlBlock, 1 << 13);
	if (FADT->PM1bControlBlock != 0) {
		outw((unsigned int) FADT->PM1bControlBlock, 1 << 13);
	}
	return 1;
}

void acpi_init() {
	acpi_find();
	if (!FADT) {
		return;
	}
	outb(FADT->SMI_CommandPort, FADT->AcpiEnable);
	acpi_working = 1;
}
