#define _WIN32_WINNT 0x0500

#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <ntddscsi.h>
#include <ata.h>

#include "ata_defs.h"
#include "stdint.h"


#define print_err(x) fprintf(stderr, "[ERROR] file= %s, ln= %d, error= %d\n", __FILE__, __LINE__, x);

uint32_t BLOCK_SIZE = 512;
uint32_t BUFFER_ALIGN = 0x4000;

char disk_model[40] = {0};
HANDLE device_handle = 0;

void dump_buffer(void* pBuffer, uint32_t ulBufferLength)
{
   uint32_t* pBuffer_DwordPtr = (uint32_t*)pBuffer;
   uint32_t  ulBufferIndex = 0;

   for (ulBufferIndex = 0; ulBufferIndex < ulBufferLength / sizeof(uint32_t); ulBufferIndex++)
   {
      if (0 == (ulBufferIndex % 0x04))
         fprintf(stdout, "\n[%8.8X]  ", ulBufferIndex * sizeof(uint32_t));

      fprintf(stdout, "%8.8X ", pBuffer_DwordPtr[ulBufferIndex]);
   }

   fprintf(stdout, "\n");
}

void device_init(char* device_name)
{
   uint32_t error_code = NO_ERROR;

   device_handle = CreateFile(
      device_name,                         // lpFileName
      GENERIC_WRITE | GENERIC_READ,        // dwDesiredAccess
      FILE_SHARE_READ | FILE_SHARE_WRITE,  // dwShareMode
      NULL,                                // lpSecurityAttributes
      OPEN_EXISTING,                       // dwCreationDisposition
      0,                                   // dwFlagsAndAttributes
      NULL);                               // hTemplateFile

   if(device_handle == INVALID_HANDLE_VALUE) {
      // Get error code.
      error_code = GetLastError();
      print_err(error_code);
   }
}

void device_shutdown()
{
   uint32_t error_code = NO_ERROR;

   // Close our handle to the drive.
   if(CloseHandle(device_handle) == 0) {
      error_code = GetLastError();
      print_err(error_code);
   }
}

int identify_drive()
{
   DWORD count;
   char* char_ptr = 0;
   ATA_PASS_THROUGH_DIRECT ata_pass_through_direct;
   ATA_ID_INFO buffer;

   unsigned long error_code = NO_ERROR;

   if(device_handle == INVALID_HANDLE_VALUE) {

      // Get error code.
      error_code = GetLastError();
      print_err(error_code);

   } else {
      // Set up our ATA_PASS_THROUGH_DIRECT structure for an IDENTIFY DEVICE command.
      // * Set <Length> to the size of the structure (in bytes).
      // * Set <AtaFlags> to ATA_FLAGS_DATA_IN because the IDENTIFY DEVICE command is a PIO DATA IN command.
      // * Set <DataTransferLength> to the number of bytes the IDENTIFY DEVICE.
      // * Set <TimeOutValue> to the maximum number of seconds to wait IDENTIFY DEVICE command to complete.
      // * Set <DataBuffer> to the address of a buffer that contains room for <DataTransferLength> bytes.
      // * Set <CurrentTaskFile[6]> to the IDENTIFY DEVICE command code.
      memset(&ata_pass_through_direct, 0, sizeof(ATA_PASS_THROUGH_DIRECT));
      ata_pass_through_direct.Length = sizeof(ATA_PASS_THROUGH_DIRECT);
      ata_pass_through_direct.AtaFlags = ATA_FLAGS_DATA_IN;
      ata_pass_through_direct.DataTransferLength = BLOCK_SIZE;
      ata_pass_through_direct.TimeOutValue = 5;
      ata_pass_through_direct.DataBuffer = &buffer;
      ata_pass_through_direct.CurrentTaskFile[DEVICE_HEAD_REGISTER] = 0xE0;
      ata_pass_through_direct.CurrentTaskFile[COMMAND_REGISTER] = IDE_COMMAND_IDENTIFY;

      // Request the device driver to execute the IDENTIFY DEVICE command.
      if(DeviceIoControl(device_handle,                     // hDevice,
         IOCTL_ATA_PASS_THROUGH_DIRECT,    // dwIoControlCode
         &ata_pass_through_direct,         // lpInBuffer
         sizeof(ATA_PASS_THROUGH_DIRECT),  // nInBufferSize
         &ata_pass_through_direct,         // lpOutBuffer
         sizeof(ATA_PASS_THROUGH_DIRECT),  // nOutBufferSize
         &count,                           // lpBytesReturned
         NULL) == 0) {                     // lpOverlapped

            // Get error code.
            error_code = GetLastError();
            print_err(error_code);

      } else {
         //      The device driver is supposed to set <count> to the size of the ATA_PASS_THROUGH_DIRECT structure.
         assert(count == sizeof(ATA_PASS_THROUGH_DIRECT));

         //      Make sure the ATA status register contains the success code of 0x50.
         if(ata_pass_through_direct.CurrentTaskFile[STATUS_REGISTER] == IDE_STATUS_IDLE) {

            // Swap Identify Buffer.  Strip trailing whitespace.
            swapIdentifyBuffer(&buffer);

            // Send the model name to standard output.
            char_ptr = ((char*)buffer.model) + sizeof(buffer.model);
            while (*--char_ptr == 0x20) *char_ptr = '\0';
            strcpy(disk_model, (char*) buffer.model);
         }
      }
   }

   return error_code;
}

uint32_t ata_read_dma(uint32_t logical_block_address, uint32_t sector_count)
{
   DWORD transfer_count;
   ATA_PASS_THROUGH_DIRECT ata_pass_through_direct;
   void*    buf_ptr;
   void*    buf_ptr_aligned;
   uint32_t buf_len;


   HANDLE hDevice;
   DWORD dwIoControlCode;
   LPVOID lpInBuffer;
   DWORD nInBufferSize;
   LPVOID lpOutBuffer;
   DWORD nOutBufferSize;
   LPDWORD lpBytesReturned;
   LPOVERLAPPED lpOverlapped;
   unsigned long error_code = NO_ERROR;
   int32_t return_code = -1;

   // Check device handle.
   if(device_handle == INVALID_HANDLE_VALUE) {

      // Get error code.
      error_code = GetLastError();
      print_err(error_code);

   }
   else
   {
      // Split lba value into bytes
      union {
         uint32_t value32;
         uint8_t  value8[4];
      } lba_packed;
      lba_packed.value32 = logical_block_address;

      buf_len = sector_count * BLOCK_SIZE;
      buf_ptr = malloc(buf_len + BUFFER_ALIGN);
      buf_ptr_aligned = (void*)(((uint32_t)buf_ptr + BUFFER_ALIGN) - ((uint32_t)buf_ptr % BUFFER_ALIGN));

      // Populate the ATA Pass through direct structure.
      memset(&ata_pass_through_direct, 0, sizeof(ATA_PASS_THROUGH_DIRECT));
      ata_pass_through_direct.Length = sizeof(ATA_PASS_THROUGH_DIRECT);
      ata_pass_through_direct.AtaFlags = (ATA_FLAGS_USE_DMA | ATA_FLAGS_DATA_IN | ATA_FLAGS_DRDY_REQUIRED);
      ata_pass_through_direct.DataTransferLength = buf_len;
      ata_pass_through_direct.TimeOutValue = 5;
      ata_pass_through_direct.DataBuffer = buf_ptr_aligned;

      // Populate task file parameters.
      ata_pass_through_direct.CurrentTaskFile[FEATURES_REGISTER]      = 0;
      ata_pass_through_direct.CurrentTaskFile[SECTOR_COUNT_REGISTER]  = (uint8_t)(sector_count & 0xFF);
      ata_pass_through_direct.CurrentTaskFile[SECTOR_NUMBER_REGISTER] = lba_packed.value8[0];
      ata_pass_through_direct.CurrentTaskFile[CYLINDER_LOW_REGISTER]  = lba_packed.value8[1];
      ata_pass_through_direct.CurrentTaskFile[CYLINDER_HIGH_REGISTER] = lba_packed.value8[2];
      ata_pass_through_direct.CurrentTaskFile[DEVICE_HEAD_REGISTER]   = (1 << 6);
      ata_pass_through_direct.CurrentTaskFile[COMMAND_REGISTER]       = IDE_COMMAND_READ_DMA;
      ata_pass_through_direct.CurrentTaskFile[RESERVED]               = 0;

      // Set function variables for DeviceIoControl.
      hDevice           = device_handle;
      dwIoControlCode   = IOCTL_ATA_PASS_THROUGH_DIRECT;
      lpInBuffer        = &ata_pass_through_direct;
      nInBufferSize     = sizeof(ATA_PASS_THROUGH_DIRECT);
      lpOutBuffer       = &ata_pass_through_direct;
      nOutBufferSize    = sizeof(ATA_PASS_THROUGH_DIRECT);
      lpBytesReturned   = &transfer_count;
      lpOverlapped      = NULL;

      // Issue the ATA Pass Through Direct command.
      if(DeviceIoControl(
         hDevice,
         dwIoControlCode,
         lpInBuffer,
         nInBufferSize,
         lpOutBuffer,
         nOutBufferSize,
         lpBytesReturned,
         lpOverlapped) == 0)
      {
         // Get error code.
         error_code = GetLastError();
         print_err(error_code);
      }

      else if (ata_pass_through_direct.CurrentTaskFile[STATUS_REGISTER] == 0x50)
      {
         return_code = 0;
         // dump_buffer(buf_ptr_aligned, buf_len);
      }

      free(buf_ptr);
   }

   return return_code;
}


int32_t ata_write_dma(uint32_t logical_block_address, uint32_t sector_count, void* source_buffer)
{
   DWORD transfer_count;
   ATA_PASS_THROUGH_DIRECT ata_pass_through_direct;
   void*    buf_ptr;
   void*    buf_ptr_aligned;
   uint32_t buf_len;

   HANDLE hDevice;
   DWORD dwIoControlCode;
   LPVOID lpInBuffer;
   DWORD nInBufferSize;
   LPVOID lpOutBuffer;
   DWORD nOutBufferSize;
   LPDWORD lpBytesReturned;
   LPOVERLAPPED lpOverlapped;
   unsigned long error_code = NO_ERROR;
   int32_t return_code = -1;

   // Check device handle.
   if(device_handle == INVALID_HANDLE_VALUE) {

      // Get error code.
      error_code = GetLastError();
      print_err(error_code);

   }
   else
   {
      // Split lba value into bytes
      union {
         uint32_t value32;
         uint8_t  value8[4];
      } lba_packed;
      lba_packed.value32 = logical_block_address;

      // Allocate buffer.
      buf_len = sector_count * BLOCK_SIZE;
      buf_ptr = malloc(buf_len + BUFFER_ALIGN);
      buf_ptr_aligned = (void*)(((uint32_t)buf_ptr + BUFFER_ALIGN) - ((uint32_t)buf_ptr % BUFFER_ALIGN));

      // Copy source buffer into aligned buffer.
      memcpy(buf_ptr_aligned, source_buffer, buf_len);

      // Populate the ATA Pass through direct structure.
      memset(&ata_pass_through_direct, 0, sizeof(ATA_PASS_THROUGH_DIRECT));
      ata_pass_through_direct.Length = sizeof(ATA_PASS_THROUGH_DIRECT);
      ata_pass_through_direct.AtaFlags = (ATA_FLAGS_USE_DMA | ATA_FLAGS_DATA_OUT | ATA_FLAGS_DRDY_REQUIRED);
      ata_pass_through_direct.DataTransferLength = buf_len;
      ata_pass_through_direct.TimeOutValue = 5;
      ata_pass_through_direct.DataBuffer = buf_ptr_aligned;

      // Populate task file parameters.
      ata_pass_through_direct.CurrentTaskFile[FEATURES_REGISTER]      = 0;
      ata_pass_through_direct.CurrentTaskFile[SECTOR_COUNT_REGISTER]  = (uint8_t)(sector_count & 0xFF);
      ata_pass_through_direct.CurrentTaskFile[SECTOR_NUMBER_REGISTER] = lba_packed.value8[0];
      ata_pass_through_direct.CurrentTaskFile[CYLINDER_LOW_REGISTER]  = lba_packed.value8[1];
      ata_pass_through_direct.CurrentTaskFile[CYLINDER_HIGH_REGISTER] = lba_packed.value8[2];
      ata_pass_through_direct.CurrentTaskFile[DEVICE_HEAD_REGISTER]   = (1 << 6);
      ata_pass_through_direct.CurrentTaskFile[COMMAND_REGISTER]       = IDE_COMMAND_WRITE_DMA;
      ata_pass_through_direct.CurrentTaskFile[RESERVED]               = 0;

      // Set function variables for DeviceIoControl.
      hDevice           = device_handle;
      dwIoControlCode   = IOCTL_ATA_PASS_THROUGH_DIRECT;
      lpInBuffer        = &ata_pass_through_direct;
      nInBufferSize     = sizeof(ATA_PASS_THROUGH_DIRECT);
      lpOutBuffer       = &ata_pass_through_direct;
      nOutBufferSize    = sizeof(ATA_PASS_THROUGH_DIRECT);
      lpBytesReturned   = &transfer_count;
      lpOverlapped      = NULL;

      // Issue the ATA Pass Through Direct command.
      if(DeviceIoControl(
         hDevice,
         dwIoControlCode,
         lpInBuffer,
         nInBufferSize,
         lpOutBuffer,
         nOutBufferSize,
         lpBytesReturned,
         lpOverlapped) == 0)
      {
         // Get error code.
         error_code = GetLastError();
         print_err(error_code);
      }

      else if (ata_pass_through_direct.CurrentTaskFile[STATUS_REGISTER] == 0x50)
      {
         return_code = 0;
         //dump_buffer(buf_ptr_aligned, buf_len);
      }

      free(buf_ptr);
   }

   return return_code;
}
