#include <Windows.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory.h>

#include "ata_commands.h"
#include "hr_time.h"
#include "stdint.h"

typedef struct {
   int      write;
   uint32_t lba;
   uint32_t  count;
   double   time;
} command_params;

/* Timestampping function. */
void TimestampLog(char* szMessage, ...)
{
   va_list hArgList;
   SYSTEMTIME hLocalTime;
   GetLocalTime(&hLocalTime);

   printf("[%02d:%02d:%02d.%03d] ",
      hLocalTime.wHour,
      hLocalTime.wMinute,
      hLocalTime.wSecond,
      hLocalTime.wMilliseconds);

   va_start(hArgList, szMessage);
   vprintf (szMessage, hArgList);
   va_end(hArgList);
}



int process_input_file(char* szFilename, command_params** ppCmdList, uint32_t* pCmdCount)
{
   FILE*       pFile = 0;
   int         nStatus = 0;
   int         nReturnValue = 0;

   char        cCmdType = 0;
   uint32_t    ulCmdLba = 0;
   uint32_t    ulCmdSectorCount = 0;

   command_params* pCmdList = 0;
   uint32_t ulCmdIndex = 0;
   uint32_t ulCmdCount = 0;


   // Open the file.
   if(0 == (pFile = fopen(szFilename, "r")))
   {
      fprintf(stderr, "fopen() failed.");
      nReturnValue = -1;
   }

   // Process the buffer
   else
   {

      // Count number of command entries are in input file.
      nStatus = 0;
      while (nStatus != EOF)
      {
         nStatus = fscanf(pFile, "%c,%08x,%03x\n", &cCmdType, &ulCmdLba, &ulCmdSectorCount);
         if ((nStatus == 3) && (cCmdType == 'R' || cCmdType == 'W'))
            ++ulCmdCount;
      }

      // Allocate command list.
      pCmdList = calloc(ulCmdCount, sizeof(command_params));

      // Seek back to start of file.
      fseek (pFile, 0x0, SEEK_SET);

      // Populate command list with command entries.
      nStatus = 0;
      while (nStatus != EOF)
      {
         nStatus = fscanf(pFile, "%c,%08x,%03x\n", &cCmdType, &ulCmdLba, &ulCmdSectorCount);
         if ((nStatus == 3) && (cCmdType == 'R' || cCmdType == 'W'))
         {
            pCmdList[ulCmdIndex].write = (cCmdType == 'W' ? 1 : 0);
            pCmdList[ulCmdIndex].lba   = ulCmdLba;
            pCmdList[ulCmdIndex].count = ulCmdSectorCount;
            ++ulCmdIndex;
         }
      }

   }

   // Close the file.
   if((pFile != 0) && (0 != (nStatus = fclose(pFile))))
   {
      fprintf(stderr, "fclose() failed.\n");
      nReturnValue = -1;
   }

   // Set return variable.
   *pCmdCount = ulCmdCount;
   *ppCmdList = pCmdList;

   return nReturnValue;
}

enum ACCESS_MODE_ENUM
{
   READ_ACCESS  = (1 << 1),
   WRITE_ACCESS = (1 << 2),
};

int main(int argc, char* argv[])
{
   int nErrorCode = NO_ERROR;
   stopWatch hHighResTimer;

   command_params* pCmdList = 0;
   uint32_t ulCmdIndex = 0;
   uint32_t ulCmdCount = 0;
   uint8_t  ucRandByte = 0;
   int32_t  nStatus = 0;

   char* szDeviceName;
   char* szInputFilename;

   void*    pSourceBuffer = 0;
   uint32_t ulSourceBufferLength = 0;

   uint32_t ulAccessMode = 0;

   // Parse input filename.
   if (argc != 4)
      goto usage;

   // Set input parameters to variables.
   szDeviceName = argv[1];
   szInputFilename = argv[2];

   // Parse access mode.
   if (2 != strlen(argv[3]))
      goto usage;

   else if (0 == strncmp(argv[3], "ro", 2))
      ulAccessMode = READ_ACCESS;

   else if (0 == strncmp(argv[3], "rw", 2))
      ulAccessMode = (READ_ACCESS | WRITE_ACCESS);

   else
      goto usage;

   // Allocate source buffer.
   ulSourceBufferLength = 512 * 256;
   pSourceBuffer = malloc(ulSourceBufferLength);

   // Populate buffer with random data.
   srand ( (uint32_t) time(NULL) );
   ucRandByte = (uint8_t) (rand() % 256);
   memset(pSourceBuffer, ucRandByte, ulSourceBufferLength);

   // Parse input file.
   process_input_file(szInputFilename, &pCmdList, &ulCmdCount);

   // Initialize device handle
   device_init(szDeviceName);

   // Issue commands per input file.
   for (ulCmdIndex = 0; ulCmdIndex < ulCmdCount; ++ulCmdIndex)
   {
      startTimer( &hHighResTimer );
      if (pCmdList[ulCmdIndex].write == 1)
      {
         if (0 != (ulAccessMode & WRITE_ACCESS))
            nStatus = ata_write_dma(pCmdList[ulCmdIndex].lba, pCmdList[ulCmdIndex].count, pSourceBuffer);
      }
      else
      {
         if (0 != (ulAccessMode & READ_ACCESS))
            nStatus = ata_read_dma(pCmdList[ulCmdIndex].lba, pCmdList[ulCmdIndex].count);
      }
      stopTimer( &hHighResTimer );
      pCmdList[ulCmdIndex].time = getElapsedTime(&hHighResTimer);

      if (((pCmdList[ulCmdIndex].write == 1) && (0 != (ulAccessMode & WRITE_ACCESS))) ||
          ((pCmdList[ulCmdIndex].write == 0) && (0 != (ulAccessMode & READ_ACCESS))))
      {
         // Set elapsed time (if command did not fail.
         if (nStatus != 0)
         {
            fprintf(stderr, "Error:  %5s @ [%8.8X , %8.8X]\n",
               (pCmdList[ulCmdIndex].write == 1 ? "Write" : "Read"),
               (pCmdList[ulCmdIndex].lba),
               (pCmdList[ulCmdIndex].lba + pCmdList[ulCmdIndex].count));
         }
         else
         {
            fprintf(stdout, "%5s @ [%8.8X , %8.8X] = %7.6f\n",
               (pCmdList[ulCmdIndex].write == 1 ? "Write" : "Read"),
               (pCmdList[ulCmdIndex].lba),
               (pCmdList[ulCmdIndex].lba + pCmdList[ulCmdIndex].count),
               (pCmdList[ulCmdIndex].time));
         }
      }
   }

   // Shutdown device handle.
   device_shutdown();

   // Exit main.
   goto exit;

usage:
   fprintf(stderr, "Usage: %s [Device Name] [Input File] [Access Mode]\n", argv[0]);
   fprintf(stderr, " [Device Name] Format:  '\\\\.\\PHYSICALDRIVE0'\n");
   fprintf(stderr, "    Use 'wmic diskdrive' to determine physical drive name.\n");
   fprintf(stderr, " [Input File]  Format:  'R|W,lba(hex),sector_count(hex)'\n");
   fprintf(stderr, " [Access Mode] Options: 'ro' or 'rw'\n");

exit:
   if (pCmdList != 0)
      free (pCmdList);

   if (pSourceBuffer != 0)
      free (pSourceBuffer);

   return nErrorCode;
}