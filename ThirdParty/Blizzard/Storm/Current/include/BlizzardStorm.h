/*
 * Blizzard storm.dll functions
 *
 * Collected by Aeris. Thanks to VL and soar.
 */
#ifndef BLIZZARD_STORM_H_INCLUDED
#define BLIZZARD_STORM_H_INCLUDED

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

#include <stddef.h>
#ifdef _WIN32
#include <windows.h>
#else
typedef int BOOL, *PBOOL, *LPBOOL;
typedef unsigned long DWORD, *PDWORD, *LPDWORD;
typedef void *HANDLE;
typedef unsigned long LCID;
typedef long LONG, *PLONG, *LPLONG;
typedef void *LPOVERLAPPED;
#endif

// All unknown parameters
typedef long BLZUNKNOWN;

#ifdef __cplusplus
extern "C"
{
#endif

/* Storm file functions */

// 250 BLZSFileInit
void __stdcall BLZSFileInit();
// 251 BLZSFileAuthenticateArchive
int __stdcall BLZSFileAuthenticateArchive(HANDLE hArchive, BLZUNKNOWN a2);	// Verify (signature) file in archive
// 252 BLZSFileCloseArchive
BOOL __stdcall BLZSFileCloseArchive(HANDLE hArchive);
// 253 BLZSFileCloseFile
BOOL __stdcall BLZSFileCloseFile(HANDLE hFile);
// 254 BLZSFileDdaBegin
int __stdcall BLZSFileDdaBegin(HANDLE hFile, BLZUNKNOWN a2, BLZUNKNOWN a3);
// 255 BLZSFileDdaBeginEx
int __stdcall BLZSFileDdaBeginEx(HANDLE hFile, BLZUNKNOWN a2, BLZUNKNOWN a3, BLZUNKNOWN lDistanceToMove, BLZUNKNOWN a5, BLZUNKNOWN a6, BLZUNKNOWN a7);
// 256 BLZSFileDdaDestroy
BOOL __stdcall BLZSFileDdaDestroy();
// 257 BLZSFileDdaEnd
BOOL __stdcall BLZSFileDdaEnd(HANDLE hDirectSound);
// 258 BLZSFileDdaGetPos
BOOL __stdcall BLZSFileDdaGetPos(HANDLE hDirectSound, BLZUNKNOWN a2, BLZUNKNOWN a3);
// 259 BLZSFileDdaGetVolume
BOOL __stdcall BLZSFileDdaGetVolume(HANDLE hDirectSound, BLZUNKNOWN a2, BLZUNKNOWN a3);
// 260 BLZSFileDdaInitialize
BOOL __stdcall BLZSFileDdaInitialize(HANDLE hDirectSound);
// 261 BLZSFileDdaSetVolume
BOOL __stdcall BLZSFileDdaSetVolume(HANDLE hDirectSound, signed int bigVolume, signed int volume);
// 262 BLZSFileDestroy
BOOL __stdcall BLZSFileDestroy();
// 263 BLZSFileEnableDirectAccess
BOOL __stdcall BLZSFileEnableDirectAccess(HANDLE hFile);
// 264 BLZSFileGetFileArchive
BOOL __stdcall BLZSFileGetFileArchive(HANDLE hFile, HANDLE hArchive);
// 265 BLZSFileGetFileSize
DWORD __stdcall BLZSFileGetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);
// 266 BLZSFileOpenArchive
BOOL __stdcall BLZSFileOpenArchive(const char *archiveName, DWORD dwPriority, DWORD dwFlags, HANDLE *pArchiveHandle);
// 267 BLZSFileOpenFile
BOOL __stdcall BLZSFileOpenFile(const char *fileName, HANDLE *pFileHandle);
// 268 BLZSFileOpenFileEx
BOOL __stdcall BLZSFileOpenFileEx(HANDLE hArchive, const char *fileName, DWORD searchScope, HANDLE *pFileHandle);
// 269 BLZSFileReadFile
BOOL __stdcall BLZSFileReadFile(HANDLE hFile, void *pBuffer, DWORD nNumberOfBytesToRead, DWORD *pNumberOfBytesRead, LONG lpDistanceToMoveHigh);
// 270 BLZSFileSetBasePath
BOOL __stdcall BLZSFileSetBasePath(const char *pathName);
// 271 BLZSFileSetFilePointer
LONG __stdcall BLZSFileSetFilePointer(HANDLE hFile, LONG filePos, LONG *filePosHigh, DWORD dwMethod);
// 272 BLZSFileSetLocale
LCID __stdcall BLZSFileSetLocale(LCID lcNewLocale);
// 273 BLZSFileGetBasePath
BOOL __stdcall BLZSFileGetBasePath(char *pBasePathBuffer, size_t bufferSize);
// 274 BLZSFileSetIoErrorMode
BOOL __stdcall BLZSFileSetIoErrorMode(int mode1, int mode2);
// 275 BLZSFileGetArchiveName
BOOL __stdcall BLZSFileGetArchiveName(HANDLE hArchive, char *pNameBuffer, size_t bufferSize);
// 276 BLZSFileGetFileName
BOOL __stdcall BLZSFileGetFileName(HANDLE hFile, char *pNameBuffer, size_t bufferSize);
// 277 BLZSFileGetArchiveInfo
BOOL __stdcall BLZSFileGetArchiveInfo(HANDLE hArchive, int *a2, int *a3);
// 278 BLZSFileSetPlatform
void __stdcall BLZSFileSetPlatform(char platform);
// 279 BLZSFileLoadFile
BOOL __stdcall BLZSFileLoadFile(const char *filePath, void **ppBuffer, size_t *pSize, size_t extraSizeToAlocate, LPOVERLAPPED pOverlapped);
// 280 BLZSFileUnloadFile
BOOL __stdcall BLZSFileUnloadFile(void *pBuffer);
// 281 BLZSFileLoadFileEx
BOOL __stdcall BLZSFileLoadFileEx(HANDLE hArchive, const char *filePath, char **ppBuffer, size_t *pBufferSize, size_t extraSize, DWORD searchScope, LPOVERLAPPED pOverlapped);
// 282 BLZSFilePrioritizeRequest
void __stdcall BLZSFilePrioritizeRequest(BOOL a1, int priority);
// 283 BLZSFileCancelRequest
void __stdcall BLZSFileCancelRequest(BLZUNKNOWN a1);
// 284 BLZSFileSetAsyncBudget
void __stdcall BLZSFileSetAsyncBudget(int asyncBudget);
// 285 BLZSFileSetDataChunkSize
void __stdcall BLZSFileSetDataChunkSize(int dataChunkSize);
// 286 BLZSFileEnableSeekOptimization
void __stdcall BLZSFileEnableSeekOptimization(BOOL enable);
// 287 BLZSFileReadFileEx
// 288 BLZSFileFileExists
BOOL __stdcall BLZSFileFileExists(const char *filePath);
// 289 BLZSFileFileExistsEx
BOOL __stdcall BLZSFileFileExistsEx(HANDLE hArchive, const char *filePath, DWORD searchScope);
// 291 BLZSFileReadFileEx2
// 292 BLZSFileLoadFileEx2
BOOL __stdcall BLZSFileLoadFileEx2(HANDLE hArchive, const char *filePath, char **ppBuffer, size_t *pBufferSize, size_t extraSize, DWORD searchScope, LPOVERLAPPED pOverlapped, int a8);
// 293 BLZSFileOpenFileAsArchive
// 294 BLZSFileGetLocale
LCID __stdcall BLZSFileGetLocale();
// 295 BLZSFileRegisterLoadNotifyProc
typedef void (__stdcall *TPFFileOpenCallback)(const char *fileName, void *pUserData);
void __stdcall BLZSFileRegisterLoadNotifyProc(TPFFileOpenCallback callback, void *pUserData);
// 296 BLZSFileGetFileCompressedSize
DWORD __stdcall BLZSFileGetFileCompressedSize(HANDLE hFile, DWORD *pSizeHigh);
// 297 BLZStorm_297
BOOL __stdcall BLZStorm_297(HANDLE hFile, char *buffer, size_t bufferSize);
// 298 BLZSFileNull
void __stdcall BLZSFileNull(); // Debug function, empty in release version.
// 299 BLZSFileAuthenticateArchiveEx

/* Storm memory functions */
// 400 BLZSMemInit
void __stdcall BLZSMemInit();
// 401 BLZSMemAlloc
// flag parameter:
#define BLZSMEM_ALLOC_ZERO_FILL 0x8
#define BLZSMEM_ALLOC_ALIGN 0x4000000
#define BLZSMEM_ALLOC_ALIGN_2 0x8000000
void *__stdcall BLZSMemAlloc(size_t size, const char *sourceName, DWORD sourceLine, DWORD flag);
// 402 BLZSMemDestroy
// Already removed
BOOL __stdcall BLZSMemDestroy();
// 403 BLZSMemFree
BOOL __stdcall BLZSMemFree(void *pMemory, const char *sourceName, DWORD sourceLine, DWORD flag);
// 404 BLZSMemGetSize
int __stdcall BLZSMemGetSize(void *pMemory, const char *sourceName, DWORD sourceLine);
// 405 BLZSMemReAlloc
void *__stdcall BLZSMemReAlloc(void *pMemory, size_t size, const char *sourceName, DWORD sourceLine, DWORD flag);
// 406 BLZSMemGetInfo
void *__stdcall BLZSMemGetInfo(int *p1, int *p2, int *p3);

// 491 BLZSMemCopy
void __stdcall BLZSMemCopy(void *pDest, const void *pSource, size_t size);

/* Storm error handling */
// 465 BLZSErrSetLastError
void __stdcall BLZSErrSetLastError(DWORD dwErrCode);

/* Storm string management */
// 500 BLZSStrInit
void __stdcall BLZSStrInit();
// 501 BLZSStrCopy
char *__stdcall BLZSStrCopy(char *dest, const char *source, size_t sizeLimit);
// 502 BLZSStrHashCombine
DWORD __stdcall BLZSStrHashCombine(const char *string, DWORD flag, DWORD seed);
// 503 BLZSStrNCat
int __stdcall BLZSStrNCat(char *base, const char *source, size_t sizeLimit);
// 504 BLZSStrTokenize
void __stdcall BLZSStrTokenize(const char **string, char **token, char *buffer, const char *whitespace, size_t size);
// 505 BLZSStr505
char *__stdcall BLZSStr505(const char *string, char c, int type);    // BLZSStrPack, BLZSStrHash64, or BLZSStrHashHT
// 506 BLZSStrLen
int __stdcall BLZSStrLen(const char *string);
// 507 BLZSStrDup
char *__stdcall BLZSStrDup(const char *string);
// 508 BLZSStrCmp
int __stdcall BLZSStrCmp(const char *string1, const char *string2, size_t size);
// 509 BLZSStrCmpI
int __stdcall BLZSStrCmpI(const char *string1, const char *string2, size_t size);
// 510 BLZSStrUpper
char *__stdcall BLZSStrUpper(char *string);
// 569 BLZSStrChr
char *__stdcall BLZSStrChr(const char *string, char c);
// 570 BLZSStrChrR
char *__stdcall BLZSStrChrR(const char *string, char c);
// 571 BLZSStrChr
char *__stdcall BLZSStrChr(const char *string, char c); // Same as 569
// 572 BLZSStrChrR
char *__stdcall BLZSStrChrR(const char *string, char c); // Same as 570
// 573 BLZSStrToDouble
double __stdcall BLZSStrToDouble(const char *string);
// 574 BLZSStrToFloat
float __stdcall BLZSStrToFloat(const char *string);
// 575 BLZSStrToInt
signed int __stdcall BLZSStrToInt(const char *string);
// 576 BLZSStrToUnsigned
unsigned int __stdcall BLZSStrToUnsigned(const char *string);
// 577 BLZSStrToInt64
__int64 __stdcall BLZSStrToInt64(const char *string);
// 578 BLZSStrPrintf
int __cdecl BLZSStrPrintf(char *dest, size_t size, const char *format, ...);
// 579 BLZSStrLower
char *__stdcall BLZSStrLower(char *string);
// 580 BLZSStr580
int __stdcall BLZSStr580(int a1, int a2, int a3, int a4);     // BLZSStrPack, BLZSStrHash64, or BLZSStrHashHT
// 581 BLZSStrVPrintf
int __cdecl BLZSStrVPrintf(char *dest, size_t size, const char *format, va_list vl);
// 588 BLZSStrStr
const char *__fastcall BLZSStrStr(const char *string1, const char *stringSearch); // Same as 596
// 590 BLZSStrHash
DWORD __stdcall BLZSStrHash(const char *string);

#ifdef __cplusplus
}
#endif

#endif // BLIZZARD_STORM_H_INCLUDED
