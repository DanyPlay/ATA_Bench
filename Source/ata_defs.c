#include "ata_defs.h"

/*******************************************************************************
*
* binvert - invert the order of bytes in a buffer
*
* This routine inverts an entire buffer, byte by byte.  For example,
* the buffer {1, 2, 3, 4, 5} would become {5, 4, 3, 2, 1}.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

void binvert( char *buf, int nbytes )
{
   char *buf_end = buf + nbytes - 1;
   char temp;

   while (buf < buf_end) {
      temp       = *buf;
      *buf       = *buf_end;
      *buf_end   = temp;

      buf_end--;
      buf++;
   }
}

void swapIdentifyBuffer(ATA_ID_INFO* buf)
{
   unsigned int i; /* indexer */
   unsigned int size;

   //binvert( (char*) &buf->config,               sizeof(buf->config) );
   //binvert( (char*) &buf->obsolete1,            sizeof(buf->obsolete1) );
   //binvert( (char*) &buf->specificConfig,       sizeof(buf->specificConfig) );
   //binvert( (char*) &buf->obsolete3,            sizeof(buf->obsolete3) );
   //binvert( (char*) &buf->retired4,             sizeof(buf->retired4) );
   //binvert( (char*) &buf->retired5,             sizeof(buf->retired5) );
   //binvert( (char*) &buf->obsolete6,            sizeof(buf->obsolete6) );
   //binvert( (char*) &buf->reserved7,            sizeof(buf->reserved7) );
   //binvert( (char*) &buf->reserved8,            sizeof(buf->reserved8) );
   //binvert( (char*) &buf->retired9,             sizeof(buf->retired9) );

   for(i = 0, size = sizeof(buf->serial[i]); i < sizeof(buf->serial) / size; i++)
      binvert( (char*) &buf->serial[i], size ); /* ASCII */

   //binvert( (char*) &buf->retired20,            sizeof(buf->retired20) );
   //binvert( (char*) &buf->retired21,            sizeof(buf->retired21) );
   //binvert( (char*) &buf->obsolete22,           sizeof(buf->obsolete22) );

   for(i = 0, size = sizeof(buf->rev[i]); i< sizeof(buf->rev) / size; i++)
      binvert( (char*) &buf->rev[i], size); /* ASCII */

   for(i = 0, size = sizeof(buf->model[i]); i< sizeof(buf->model) / size; i++)
      binvert( (char*) &buf->model[i], size); /* ASCII */

   //binvert( (char*) &buf->multipleCount,         sizeof(buf->multipleCount) );
   //binvert( (char*) &buf->reserved48,            sizeof(buf->reserved48) );
   //binvert( (char*) &buf->capabilities,          sizeof(buf->capabilities) );
   //binvert( (char*) &buf->capabilities2,         sizeof(buf->capabilities2) );
   //binvert( (char*) &buf->obsolete51,            sizeof(buf->obsolete51) );
   //binvert( (char*) &buf->fieldValidity,         sizeof(buf->fieldValidity) );
   //binvert( (char*) &buf->obsolete54,            sizeof(buf->obsolete54) );
   //binvert( (char*) &buf->obsolete55,            sizeof(buf->obsolete55) );
   //binvert( (char*) &buf->obsolete56,            sizeof(buf->obsolete56) );
   //binvert( (char*) &buf->obsolete57,            sizeof(buf->obsolete57) );
   //binvert( (char*) &buf->obsolete58,            sizeof(buf->obsolete58) );
   //binvert( (char*) &buf->multiSecs,             sizeof(buf->multiSecs) );
   //binvert( (char*) &buf->sectors,               sizeof(buf->sectors) );
   //binvert( (char*) &buf->obsolete62,            sizeof(buf->obsolete62) );
   //binvert( (char*) &buf->multiDma,              sizeof(buf->multiDma) );
   //binvert( (char*) &buf->advancedPio,           sizeof(buf->advancedPio) );
   //binvert( (char*) &buf->cycletimeDma,          sizeof(buf->cycletimeDma) );
   //binvert( (char*) &buf->cycletimeMulti,        sizeof(buf->cycletimeMulti) );
   //binvert( (char*) &buf->cycletimePioNoIordy,   sizeof(buf->cycletimePioNoIordy) );
   //binvert( (char*) &buf->cycletimePioIordy,     sizeof(buf->cycletimePioIordy) );
   //binvert( (char*) &buf->reserved69,            sizeof(buf->reserved69) );
   //binvert( (char*) &buf->reserved70,            sizeof(buf->reserved70) );
   //binvert( (char*) &buf->reserved71,            sizeof(buf->reserved71) );
   //binvert( (char*) &buf->queueDepth,            sizeof(buf->queueDepth) );
   //binvert( (char*) &buf->sataCapabilities,      sizeof(buf->sataCapabilities) );
   //binvert( (char*) &buf->reserved77,            sizeof(buf->reserved77) );
   //binvert( (char*) &buf->sataFeatSupport,       sizeof(buf->sataFeatSupport) );
   //binvert( (char*) &buf->sataFeatEnabled,       sizeof(buf->sataFeatEnabled) );
   //binvert( (char*) &buf->majorRevNum,           sizeof(buf->majorRevNum) );
   //binvert( (char*) &buf->minorVersNum,          sizeof(buf->minorVersNum) );
   //binvert( (char*) &buf->cmdSetSupport0,        sizeof(buf->cmdSetSupport0) );
   //binvert( (char*) &buf->cmdSetSupport1,        sizeof(buf->cmdSetSupport1) );
   //binvert( (char*) &buf->cmdSetSupportExt,      sizeof(buf->cmdSetSupportExt) );
   //binvert( (char*) &buf->cmdSetEnabled0,        sizeof(buf->cmdSetEnabled0) );
   //binvert( (char*) &buf->cmdSetEnabled1,        sizeof(buf->cmdSetEnabled1) );
   //binvert( (char*) &buf->cmdSetDefault,         sizeof(buf->cmdSetDefault) );
   //binvert( (char*) &buf->ultraDmaMode,          sizeof(buf->ultraDmaMode) );
   //binvert( (char*) &buf->secEraseTime,          sizeof(buf->secEraseTime) );
   //binvert( (char*) &buf->enhancedSecEraseTime,  sizeof(buf->enhancedSecEraseTime) );
   //binvert( (char*) &buf->currentAdvPowerMgt,    sizeof(buf->currentAdvPowerMgt) );
   //binvert( (char*) &buf->mstrPasswdRevision,    sizeof(buf->mstrPasswdRevision) );
   //binvert( (char*) &buf->comResetResult,        sizeof(buf->comResetResult) );
   //binvert( (char*) &buf->acousticMgmt,          sizeof(buf->acousticMgmt) );
   //binvert( (char*) &buf->streamMinReqSize,      sizeof(buf->streamMinReqSize) );
   //binvert( (char*) &buf->streamTransferTimeDma, sizeof(buf->streamTransferTimeDma) );
   //binvert( (char*) &buf->streamAccessLatency,   sizeof(buf->streamAccessLatency) );
   //binvert( (char*) &buf->streamPerfGranularity, sizeof(buf->streamPerfGranularity) );
   //binvert( (char*) &buf->maxLBA,                sizeof(buf->maxLBA) );
   //binvert( (char*) &buf->streamTransferTimePio, sizeof(buf->streamTransferTimePio) );
   //binvert( (char*) &buf->reserved105,           sizeof(buf->reserved105) );
   //binvert( (char*) &buf->physSecSize,           sizeof(buf->physSecSize) );
   //binvert( (char*) &buf->interseekDelay,        sizeof(buf->interseekDelay) );
   //binvert( (char*) &buf->naaIeeeOui,            sizeof(buf->naaIeeeOui) );
   //binvert( (char*) &buf->ieeeOuiUniqueId,       sizeof(buf->ieeeOuiUniqueId) );
   //binvert( (char*) &buf->uniqueId1,             sizeof(buf->uniqueId1) );
   //binvert( (char*) &buf->uniqueId2,             sizeof(buf->uniqueId2) );
   //binvert( (char*) &buf->worldWideNameExt,      sizeof(buf->worldWideNameExt) );
   //binvert( (char*) &buf->techSupport,           sizeof(buf->techSupport) );
   //binvert( (char*) &buf->wordsPerSector,        sizeof(buf->wordsPerSector) );
   //binvert( (char*) &buf->reserved119[0],        sizeof(buf->reserved119) / sizeof(buf->reserved119[0]) );
   //binvert( (char*) &buf->remMediaStatus,        sizeof(buf->remMediaStatus) );
   //binvert( (char*) &buf->secStatus,             sizeof(buf->secStatus) );

   for(i = 0, size = sizeof(buf->vendorStatus[0]); i < sizeof(buf->vendorStatus) / size; i++)
      binvert( (char*) &buf->vendorStatus[i], size ); /* Vendor Specific Fields (UNKNOWN SIZES) */

   //binvert( (char*) &buf->cfaPowerMode1,         sizeof(buf->cfaPowerMode1) );
   //binvert( (char*) &buf->reserved161[0],        sizeof(buf->reserved161) / sizeof(buf->reserved161[0]) );

   for(i = 0, size = sizeof(buf->mediaSerialNum[0]); i< sizeof(buf->mediaSerialNum) / size; i++)
      binvert( (char*) &buf->mediaSerialNum[i], size ); /* ASCII */

   //binvert( (char*) &buf->reserved206[0],        sizeof(buf->reserved206) / sizeof(buf->reserved206[0]) );
   //binvert( (char*) &buf->integrity,             sizeof(buf->integrity) );
}
