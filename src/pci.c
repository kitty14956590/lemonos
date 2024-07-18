#include <pci.h>
#include <linked.h>
#include <memory.h>
#include <stdint.h>
#include <ports.h>

linked_t * pci_devices;

void pci_add(pci_t * device) {
	linked_t * branch = linked_add(pci_devices, device);
	if (!branch) {
		return;
	}
	pci_devices = branch;
}

int pci_exists(uint8_t bus, uint8_t slot, uint8_t function, uint16_t vendor, uint16_t device) {
	linked_t * node = pci_devices;
	while (node->next) {
		pci_t * pci_device = (pci_t *) node->p;
		if (pci_device->bus == bus && pci_device->slot == slot && pci_device->function == function && pci_device->vendor == vendor && pci_device->device == device) {
			return 1;
		}
		node = node->next;
	}
	return 0;
}

pci_t * pci_get(uint8_t bus, uint8_t slot, uint8_t function) {
	linked_t * node = pci_devices;
	while (node->next) {
		pci_t * pci_device = (pci_t *) node->p;
		if (pci_device->bus == bus && pci_device->slot == slot && pci_device->function == function) {
			return pci_device;
		}
		node = node->next;
	}
	return 0;
}

uint16_t pci_config_inw(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset) {
	uint32_t address = (uint32_t) ((bus << 16) | (slot << 11) | (function << 8) | (offset & 0xfc) | (0x80000000));
	outl(0xcf8, address);
	return inl(0xcfc) >> ((offset & 2) * 8);
}

uint32_t pci_config_ind(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset) {
	uint32_t address = (uint32_t) ((bus << 16) | (slot << 11) | (function << 8) | (offset & 0xfc) | (0x80000000));
	outl(0xcf8, address);
	return inl(0xcfc);
}

uint32_t pci_config_outd(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint32_t d) {
	uint32_t address = (uint32_t) ((bus << 16) | (slot << 11) | (function << 8) | (offset & 0xfc) | (0x80000000));
	outl(0xcf8, address);
	outl(0xcfc, d);
}

void pci_probe() {
	uint32_t bus = 0, slot = 0, function = 0;
	while (bus < 256) {
		while (slot < 32) {
			while (function < 8) {
				pci_t * pci_device;
				uint16_t vendor = pci_config_inw(bus, slot, function, 0);
				if (vendor == 0xffff) {
					function++;
					continue;
				}
				uint16_t device = pci_config_inw(bus, slot, function, 2);
				uint16_t class = pci_config_inw(bus, slot, function, 8);
				pci_device = malloc(sizeof(pci_t) + 8);
				pci_device->vendor = vendor;
				pci_device->device = device;
				pci_device->class = class & 0xff;
				pci_device->subclass = (class >> 8) & 0xff;
				pci_device->function = function;
				pci_device->slot = slot;
				pci_device->bus = bus;
				pci_add(pci_device);
				function++;
			}
			slot++;
		}
		bus++;
	}
}