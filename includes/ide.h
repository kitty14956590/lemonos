#pragma once

#include <stdint.h>

#define ATA_STATUS_BUSY 0x80
#define ATA_STATUS_READY 0x40
#define ATA_STATUS_DRIVE_FAIL 0x20
#define ATA_STATUS_SEEK_COMPLETE 0x10
#define ATA_STATUS_READY_REQ 0x08
#define ATA_STATUS_CORRECTED 0x04
#define ATA_STATUS_INDEX 0x02
#define ATA_STATUS_ERROR 0x01

#define ATA_ERROR_BAD  0x80
#define ATA_ERROR_UNCORRECTED 0x40
#define ATA_ERROR_MEDIA_CHANGED   0x20
#define ATA_ERROR_MARK_NOT_FOUND 0x10
#define ATA_ERROR_MEDIA_CHANGE_REQ  0x08
#define ATA_ERROR_ABORT 0x04
#define ATA_ERROR_TRACK_NOT_FOUND 0x02
#define ATA_ERROR_ADDRESS_NOT_FOUND 0x01

#define ATA_CMD_READ_PIO 0x20
#define ATA_CMD_XREAD_PIO 0x24
#define ATA_CMD_XREAD_DMA 0x25
#define ATA_CMD_WRITE_PIO 0x30
#define ATA_CMD_XWRITE_PIO 0x34
#define ATA_CMD_XWRITE_DMA 0x35
#define ATA_CMD_PACKET 0xa0
#define ATA_CMD_IDENTIFY_PACKET 0xa1
#define ATA_CMD_READ_DMA 0xc8
#define ATA_CMD_WRITE_DMA 0xca
#define ATA_CMD_CACHE_FLUSH 0xe7
#define ATA_CMD_XCACHE_FLUSH 0xea
#define ATA_CMD_IDENTIFY 0xec

#define ATAPI_CMD_EJECT 0x1b
#define ATAPI_CMD_READ 0xa8

#define ATA_IDENTIFY_TYPE 0
#define ATA_IDENTIFY_CYLINDERS 2
#define ATA_IDENTIFY_HEADS 6
#define ATA_IDENTIFY_SECTORS 12
#define ATA_IDENTIFY_SERIAL 20
#define ATA_IDENTIFY_MODEL 54
#define ATA_IDENTIFY_CAPABILITIES 98
#define ATA_IDENTIFY_FIELD 106
#define ATA_IDENTIFY_MAX_LBA 120
#define ATA_IDENTIFY_COMMANDSETS 164
#define ATA_IDENTIFY_XMAX_LBA 200

enum {
	ATA_REG_DATA,
	ATA_REG_ERROR,
	ATA_REG_FEATURES = 1,
	ATA_REG_SECCOUNT0,
	ATA_REG_LBA0,
	ATA_REG_LBA1,
	ATA_REG_LBA2,
	ATA_REG_SELECT,
	ATA_REG_COMMAND,
	ATA_REG_STATUS = 7,
	ATA_REG_SECCOUNT1,
	ATA_REG_LBA3,
	ATA_REG_LBA4,
	ATA_REG_LBA5,
	ATA_REG_CONTROL,
	ATA_REG_ALTSTATUS = 13,
	ATA_REG_ADDRESS,
};

enum {
	ATA_PRIMARY,
	ATA_SECONDARY,
};

enum {
	ATA_READ,
	ATA_WRITE,
};

enum {
	IDE_ATA,
	IDE_ATAPI,
};

enum {
	ATA_MASTER,
	ATA_SLAVE,
};

typedef struct {
	uint8_t exists;
	uint8_t channel;
	uint8_t master;
	uint16_t type;
	uint16_t signature;
	uint16_t capabilities;
	uint32_t commands;
	uint32_t size;
	char name[48];
} ide_device_t;

extern uint8_t * ide_buffer;
extern void * ide_void_buffer;
extern ide_device_t ide_devices[4];

void ide_init();