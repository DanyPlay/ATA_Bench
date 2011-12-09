#ifndef __ATA_DEFS__INCLUDED__
#define __ATA_DEFS__INCLUDED__

#include "stdint.h"

typedef enum ataRegisterEnum
{
   ERROR_REGISTER         = 0,
   FEATURES_REGISTER      = 0,
   SECTOR_COUNT_REGISTER  = 1,
   SECTOR_NUMBER_REGISTER = 2,
   CYLINDER_LOW_REGISTER  = 3,
   CYLINDER_HIGH_REGISTER = 4,
   DEVICE_HEAD_REGISTER   = 5,
   STATUS_REGISTER        = 6,
   COMMAND_REGISTER       = 6,
   RESERVED               = 7,

} ATA_REGISTER_ENUM;


#pragma pack(push)
#pragma pack(1)

/* This structure is defined by the ATAPI-6 spec + the SATA specification */
typedef struct ataIdInfo
{
   uint16_t config;               /*002 [000-000] general configuration */
   uint16_t obsolete1;            /*004 [001-001] obsolete (cylinders)*/
   uint16_t specificConfig;       /*006 [002-002] Specific configuration (removcyl)*/
   uint16_t obsolete3;            /*008 [003-003] obsolete (heads) */
   uint16_t retired4;             /*010 [004-004] retired (bytesTrack) */
   uint16_t retired5;             /*012 [005-005] retired (bytesSec) */
   uint16_t obsolete6;            /*014 [006-006] obsolete (sectors)*/
   uint16_t reserved7;            /*016 [007-007] reserved for assignment by CFA (bytesGap) */
   uint16_t reserved8;            /*018 [008-008] reserved for assignment by CFA (bytesSync)*/
   uint16_t retired9;             /*020 [009-009] retired  (vendStat)*/
   uint16_t serial[10];           /*040 [010-019] controller serial number */
   uint16_t retired20;            /*044 [020-020] retired (type)*/
   uint16_t retired21;            /*044 [021-021] retired (size)*/
   uint16_t obsolete22;           /*046 [022-022] obsolete (bytesEcc)*/
   uint16_t rev[4];               /*054 [023-026] firmware revision */
   uint16_t model[20];            /*094 [027-046] model name */
   uint16_t multipleCount;        /*096-[047-047] multiple count (multiSecs)*/
   uint16_t reserved48;           /*098 [048-048] reserved */
   uint16_t capabilities;         /*100-[049-049] capabilities */
   uint16_t capabilities2;        /*102-[050-050] capabilities */
   uint32_t obsolete51;           /*106 [051-052] obsolete */
   uint16_t fieldValidity;        /*108-[053-053] Field Validity */
   uint16_t obsolete54;           /*118 [054-054] obsolete (currentCyl)  */
   uint16_t obsolete55;           /*118 [055-055] obsolete (currentHeads)*/
   uint16_t obsolete56;           /*118 [056-056] obsolete (currentSectors) */
   uint16_t obsolete57;           /*118 [057-057] obsolete */
   uint16_t obsolete58;           /*119 [058-058] obsolete */
   uint16_t multiSecs;            /*120 [059-059] sector configuration */
   uint32_t sectors;              /*124 [060-061] total number of user addressable sectors */
   uint16_t obsolete62;           /*126 [062-062] obsolete */
   uint16_t multiDma;             /*128-[063-063] multi word DMA transfer */
   uint16_t advancedPio;          /*130-[064-064] flow control PIO transfer modes supported */
   uint16_t cycletimeDma;         /*132-[065-065] minimum multiword DMA transfer cycle time */
   uint16_t cycletimeMulti;       /*134-[066-066] recommended multiword DMA cycle time */
   uint16_t cycletimePioNoIordy;  /*136-[067-067] min PIO transfer cycle time wo flow ctl */
   uint16_t cycletimePioIordy;    /*138-[068-068] min PIO transfer cycle time w IORDY */
   uint16_t reserved69;           /*142 [069-069] reserved for command queuing */
   uint16_t reserved70;           /*144 [070-070] reserved for command overlap */
   uint64_t reserved71;           /*150 [071-072] reserved for IDENTIFY PACKET DEVICE */
   uint16_t queueDepth;           /*152-[075-075] Queue Depth */
   uint16_t sataCapabilities;     /*154-[076-076] SATA capabilities */
   uint16_t reserved77;           /*156-[077-077] reserved for future SATA definition */
   uint16_t sataFeatSupport;      /*158-[078-078] SATA features supported */
   uint16_t sataFeatEnabled;      /*160-[079-079] SATA features enabled */
   uint16_t majorRevNum;          /*162 [080-080] Major Revision Number (0|FFFF if no) */
   uint16_t minorVersNum;         /*164 [081-081] Minor  Version Number (0|FFFF if no) */
   uint16_t cmdSetSupport0;       /*166 [082-082] Command Set Supported */
   uint16_t cmdSetSupport1;       /*168 [083-083] Command Set Supported */
   uint16_t cmdSetSupportExt;     /*170 [084-084] Command Set/Feature Support Extension */
   uint16_t cmdSetEnabled0;       /*172 [085-085] Command Set/Feature enabled */
   uint16_t cmdSetEnabled1;       /*174 [086-086] Command Set/Feature enabled */
   uint16_t cmdSetDefault;        /*176 [087-087] Command Set/Feature default */
   uint16_t ultraDmaMode;         /*178-[088-088] Ultra DMA Mode selection.support */
   uint16_t secEraseTime;         /*180 [089-089] Time required for security erase completion */
   uint16_t enhancedSecEraseTime; /*182 [090-090] Time for enhanced security erase competion */
   uint16_t currentAdvPowerMgt;   /*184 [091-091] Current advanced power management value */
   uint16_t mstrPasswdRevision;   /*186 [092-092] master password revision code */
   uint16_t comResetResult;       /*188-[093-093] COMRESET result */
   uint16_t acousticMgmt;         /*190 [094-094] acoustics mgmt */
   uint16_t streamMinReqSize;     /*192 [095-095] stream minimum request size */
   uint16_t streamTransferTimeDma;/*194 [096-096] stream transer time - dma */
   uint16_t streamAccessLatency;  /*196 [097-097] stream access latency - dma and pio */
   uint32_t streamPerfGranularity;/*200 [098-099] stream performance granularity */
   uint64_t maxLBA;               /*208 [100-103] max user LBA for 48-bit address feature */
   uint16_t streamTransferTimePio;/*210 [104-104] streaming transfer time - pio */
   uint16_t reserved105;          /*212 [105-105] reserved */
   uint16_t physSecSize;          /*214 [106-106] physical sector size / logical sector size */
   uint16_t interseekDelay;       /*216 [107-107] inter-seek delay for ISO-7779 */
   uint16_t naaIeeeOui;           /*218 [108-108] NAA (3:0) IEEE OUI (23:12) */
   uint16_t ieeeOuiUniqueId;      /*220 [109-109] IEEE OUI (11:0) unique id (35:32) */
   uint16_t uniqueId1;            /*222 [110-110] unique id (31:16) */
   uint16_t uniqueId2;            /*224 [111-111] unique id (15:0) */
   uint64_t worldWideNameExt;     /*232 [112-115] reserved for world wide name extension */
   uint16_t techSupport;          /*234 [116-116] reserved for tech support */
   uint32_t wordsPerSector;       /*238 [117-118] words per logical sector */
   uint16_t reserved119[8];       /*254 [119-126] reserved */
   uint16_t remMediaStatus;       /*256 [127-127] removable media status */
   uint16_t secStatus;            /*258 [128-128] security status */
   uint16_t vendorStatus[31];     /*320 [129-159] vendor specific status */
   uint16_t cfaPowerMode1;        /*322 [160-160] CFA Power mode 1 */
   uint16_t reserved161[15];      /*352 [161-175] reserved for assignment by CFA Ass. */
   uint16_t mediaSerialNum[30];   /*412 [176-205] current media serial number */
   uint16_t reserved206[49];      /*510 [206-254] reserved */
   uint16_t integrity;            /*512 [255-255] Integrity Word */
} ATA_ID_INFO;

#pragma pack(pop)

/* Function definitions. */
void swapIdentifyBuffer(ATA_ID_INFO* buf);

#endif /* __ATA_DEFS__INCLUDED__ */