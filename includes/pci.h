#pragma once

#include <stdint.h>

typedef struct {
	uint16_t vendor;
	uint16_t device;
	uint8_t function;
	uint8_t slot;
	uint8_t bus;
	uint8_t revision;
	uint8_t class;
	uint8_t subclass;
	uint8_t irq_line;
	uint8_t irq_pin;
	uint8_t latency;
	uint8_t grant;
	uint8_t programming;
	uint32_t bar0;
	uint32_t bar1;
	uint32_t bar2;
	uint32_t bar3;
	uint32_t bar4;
	uint32_t bar5;
	uint32_t id;
} pci_t;

typedef int (* pci_callback_t)(pci_t * device);

typedef struct {
	pci_callback_t callback;
	pci_callback_t check;
} pci_handler_t;

void pci_add(pci_t * device);
void pci_add_handler(pci_callback_t handler, pci_callback_t check);
int pci_exists(uint8_t bus, uint8_t slot, uint8_t function, uint16_t vendor, uint16_t device);
pci_t * pci_get(uint8_t bus, uint8_t slot, uint8_t function);
uint16_t pci_config_inw(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
uint32_t pci_config_ind(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);
uint32_t pci_config_outd(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint32_t d);
void pci_probe();
void pci_init();