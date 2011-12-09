#ifndef _ata_commands_h_
#define _ata_commands_h_

#include "stdint.h"

void dump_buffer(void* pBuffer, uint32_t ulBufferLength);
void device_init(char* device_name);
void device_shutdown();
int identify_drive();
int ata_read_dma(uint32_t logical_block_address, uint32_t sector_count);
int ata_write_dma(uint32_t logical_block_address, uint32_t sector_count, void* source_buffer);

#endif /* _ata_commands_h_ */