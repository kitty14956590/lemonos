#pragma once

typedef struct {
        uint16_t vendor;
        uint16_t device;
        uint8_t function;
        uint8_t slot;
        uint8_t bus;
        uint8_t revision;
        uint8_t class;
        uint8_t subclass;
        uint32_t bar0;
        uint32_t bar1;
        uint32_t bar2;
        uint32_t bar3;
        uint32_t bar4;
        uint32_t bar5;
        uint32_t id;
} pci_t;