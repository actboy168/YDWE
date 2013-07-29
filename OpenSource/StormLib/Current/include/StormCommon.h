/*****************************************************************************/
/* SCommon.h                              Copyright (c) Ladislav Zezula 2003 */
/*---------------------------------------------------------------------------*/
/* Common functions for encryption/decryption from Storm.dll. Included by    */
/* SFile*** functions, do not include and do not use this file directly      */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 24.03.03  1.00  Lad  The first version of SFileCommon.h                   */
/* 12.06.04  1.00  Lad  Renamed to SCommon.h                                 */
/* 06.09.10  1.00  Lad  Renamed to StormCommon.h                             */
/*****************************************************************************/

#ifndef __STORMCOMMON_H__
#define __STORMCOMMON_H__

//-----------------------------------------------------------------------------
// Compression support

// Include functions from Pkware Data Compression Library
#include "pklib/pklib.h"

// Include functions from Huffmann compression
#include "huffman/huff.h"

// Include functions from IMA ADPCM compression
#include "adpcm/adpcm.h"

// Include functions from SPARSE compression
#include "sparse/sparse.h"

// Include functions from LZMA compression
#include "lzma/C/LzmaEnc.h"
#include "lzma/C/LzmaDec.h"

// Include functions from zlib
#ifndef __SYS_ZLIB
  #include "zlib/zlib.h"
#else
  #include <zlib.h>
#endif

// Include functions from bzlib
#ifndef __SYS_BZLIB
  #include "bzip2/bzlib.h"
#else
  #include <bzlib.h>
#endif

//-----------------------------------------------------------------------------
// Cryptography support

// Headers from LibTomCrypt
#include "libtomcrypt/src/headers/tomcrypt.h"

// For HashStringJenkins
#include "jenkins/lookup.h"

//-----------------------------------------------------------------------------
// StormLib private defines

#define ID_MPQ_FILE            0x46494c45     // Used internally for checking TMPQFile ('FILE')

#define MPQ_WEAK_SIGNATURE_SIZE        64
#define MPQ_STRONG_SIGNATURE_SIZE     256 

// Prevent problems with CRT "min" and "max" functions,
// as they are not defined on all platforms
#define STORMLIB_MIN(a, b) ((a < b) ? a : b)
#define STORMLIB_MAX(a, b) ((a > b) ? a : b)

// Macro for building 64-bit file offset from two 32-bit
#define MAKE_OFFSET64(hi, lo)      (((ULONGLONG)hi << 32) | lo)

//-----------------------------------------------------------------------------
// StormLib internal global variables

extern DWORD dwGlobalFlags;                     // Global StormLib flags
extern LCID lcFileLocale;                       // Preferred file locale

//-----------------------------------------------------------------------------
// Encryption and decryption functions

#define MPQ_HASH_TABLE_INDEX    0x000
#define MPQ_HASH_NAME_A         0x100
#define MPQ_HASH_NAME_B         0x200
#define MPQ_HASH_FILE_KEY       0x300

DWORD HashString(const char * szFileName, DWORD dwHashType);

void InitializeMpqCryptography();

DWORD GetHashTableSizeForFileCount(DWORD dwFileCount);

bool IsPseudoFileName(const char * szFileName, LPDWORD pdwFileIndex);
ULONGLONG HashStringJenkins(const char * szFileName);

void ConvertMpqHeaderToFormat4(TMPQArchive * ha, ULONGLONG FileSize, DWORD dwFlags);

void  EncryptMpqBlock(void * pvFileBlock, DWORD dwLength, DWORD dwKey);
void  DecryptMpqBlock(void * pvFileBlock, DWORD dwLength, DWORD dwKey);

DWORD DetectFileKeyBySectorSize(LPDWORD SectorOffsets, DWORD decrypted);
DWORD DetectFileKeyByContent(void * pvFileContent, DWORD dwFileSize);
DWORD DecryptFileKey(const char * szFileName, ULONGLONG MpqPos, DWORD dwFileSize, DWORD dwFlags);

//-----------------------------------------------------------------------------
// Handle validation functions

bool IsValidMpqHandle(TMPQArchive * ha);
bool IsValidFileHandle(TMPQFile * hf);

//-----------------------------------------------------------------------------
// Hash table and block table manipulation

TMPQHash * GetFirstHashEntry(TMPQArchive * ha, const char * szFileName);
TMPQHash * GetNextHashEntry(TMPQArchive * ha, TMPQHash * pFirstHash, TMPQHash * pPrevHash);
DWORD AllocateHashEntry(TMPQArchive * ha, TFileEntry * pFileEntry);
DWORD AllocateHetEntry(TMPQArchive * ha, TFileEntry * pFileEntry);

void FindFreeMpqSpace(TMPQArchive * ha, ULONGLONG * pFreeSpacePos);

// Functions that load the HET abd BET tables
int  CreateHashTable(TMPQArchive * ha, DWORD dwHashTableSize);
int  LoadAnyHashTable(TMPQArchive * ha);
int  BuildFileTable(TMPQArchive * ha, ULONGLONG FileSize);
int  SaveMPQTables(TMPQArchive * ha);

TMPQHetTable * CreateHetTable(DWORD dwMaxFileCount, DWORD dwHashBitSize, bool bCreateEmpty);
void FreeHetTable(TMPQHetTable * pHetTable);

TMPQBetTable * CreateBetTable(DWORD dwMaxFileCount);
void FreeBetTable(TMPQBetTable * pBetTable);

// Functions for finding files in the file table
TFileEntry * GetFileEntryAny(TMPQArchive * ha, const char * szFileName);
TFileEntry * GetFileEntryLocale(TMPQArchive * ha, const char * szFileName, LCID lcLocale);
TFileEntry * GetFileEntryExact(TMPQArchive * ha, const char * szFileName, LCID lcLocale);
TFileEntry * GetFileEntryByIndex(TMPQArchive * ha, DWORD dwIndex);

// Allocates file name in the file entry
void AllocateFileName(TFileEntry * pFileEntry, const char * szFileName);

// Allocates new file entry in the MPQ tables. Reuses existing, if possible
TFileEntry * FindFreeFileEntry(TMPQArchive * ha);
TFileEntry * AllocateFileEntry(TMPQArchive * ha, const char * szFileName, LCID lcLocale);
int  RenameFileEntry(TMPQArchive * ha, TFileEntry * pFileEntry, const char * szNewFileName);
void ClearFileEntry(TMPQArchive * ha, TFileEntry * pFileEntry);
int  FreeFileEntry(TMPQArchive * ha, TFileEntry * pFileEntry);

// Invalidates entries for (listfile) and (attributes)
void InvalidateInternalFiles(TMPQArchive * ha);

//-----------------------------------------------------------------------------
// Common functions - MPQ File

TMPQFile * CreateMpqFile(TMPQArchive * ha);
int  LoadMpqTable(TMPQArchive * ha, ULONGLONG ByteOffset, void * pvTable, DWORD dwCompressedSize, DWORD dwRealSize, DWORD dwKey);
int  AllocateSectorBuffer(TMPQFile * hf);
int  AllocatePatchInfo(TMPQFile * hf, bool bLoadFromFile);
int  AllocateSectorOffsets(TMPQFile * hf, bool bLoadFromFile);
int  AllocateSectorChecksums(TMPQFile * hf, bool bLoadFromFile);
void CalculateRawSectorOffset(ULONGLONG & RawFilePos, TMPQFile * hf, DWORD dwSectorOffset);
int  WritePatchInfo(TMPQFile * hf);
int  WriteSectorOffsets(TMPQFile * hf);
int  WriteSectorChecksums(TMPQFile * hf);
int  WriteMemDataMD5(TFileStream * pStream, ULONGLONG RawDataOffs, void * pvRawData, DWORD dwRawDataSize, DWORD dwChunkSize, LPDWORD pcbTotalSize);
int  WriteMpqDataMD5(TFileStream * pStream, ULONGLONG RawDataOffs, DWORD dwRawDataSize, DWORD dwChunkSize);
void FreeMPQFile(TMPQFile *& hf);

bool IsPatchData(const void * pvData, DWORD cbData, LPDWORD pdwPatchedFileSize);
int  PatchFileData(TMPQFile * hf);

void FreeMPQArchive(TMPQArchive *& ha);

//-----------------------------------------------------------------------------
// Utility functions

bool CheckWildCard(const char * szString, const char * szWildCard);
const char * GetPlainFileNameA(const char * szFileName);
const TCHAR * GetPlainFileNameT(const TCHAR * szFileName);
bool IsInternalMpqFileName(const char * szFileName);

//-----------------------------------------------------------------------------
// Support for adding files to the MPQ

int SFileAddFile_Init(
    TMPQArchive * ha,
    const char * szArchivedName,
    ULONGLONG ft,
    DWORD dwFileSize,
    LCID lcLocale,
    DWORD dwFlags,
    TMPQFile ** phf
    );

int SFileAddFile_Write(
    TMPQFile * hf,
    const void * pvData,
    DWORD dwSize,
    DWORD dwCompression
    );

int SFileAddFile_Finish(
    TMPQFile * hf
    );

//-----------------------------------------------------------------------------
// Attributes support

int  SAttrLoadAttributes(TMPQArchive * ha);
int  SAttrFileSaveToMpq(TMPQArchive * ha);

//-----------------------------------------------------------------------------
// Listfile functions

int  SListFileSaveToMpq(TMPQArchive * ha);

//-----------------------------------------------------------------------------
// Dump data support

#ifdef __STORMLIB_DUMP_DATA__
void DumpMpqHeader(TMPQHeader * pHeader);
void DumpHetAndBetTable(TMPQHetTable * pHetTable, TMPQBetTable * pBetTable);

#else
#define DumpMpqHeader(h)           /* */
#define DumpHetAndBetTable(h, b)   /* */
#endif

#endif // __STORMCOMMON_H__

