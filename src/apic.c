/*
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
                enable_apic();
                disable_ioapic();
*/