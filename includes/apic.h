#pragma once

typedef struct _APICHeader {
	uint8_t type;
	uint8_t length;
} __attribute__ ((packed)) APICHeader_t;

typedef struct _APIC {
	ACPISDTHeader_t header;
	uint8_t processorID;
	uint8_t apicID;
	uint32_t flags;
} __attribute__ ((packed)) APIC_t;

typedef struct _IOApic {
	ACPISDTHeader_t header;
	uint8_t id;
	uint8_t reserved;
	uint32_t address;
	uint32_t base;
} __attribute__ ((packed)) IOApic_t;

void apic_init();

extern unsigned char * apic_address;
extern unsigned char * io_apic_address;
extern APIC_t * apics[32];
extern IOApic_t * IOApic;