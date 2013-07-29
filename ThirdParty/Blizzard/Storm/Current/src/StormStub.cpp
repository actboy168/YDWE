#include <BlizzardStorm.h>

// 250 BLZSFileInit
void __stdcall BLZSFileInit()
{
}

// 251 BLZSFileAuthenticateArchive
int __stdcall BLZSFileAuthenticateArchive(HANDLE hArchive, BLZUNKNOWN a2)
{
	return 0;
}
	// Verify (signature) file in archive
// 252 BLZSFileCloseArchive
BOOL __stdcall BLZSFileCloseArchive(HANDLE hArchive)
{
	return 0;
}

// 253 BLZSFileCloseFile
BOOL __stdcall BLZSFileCloseFile(HANDLE hFile)
{
	return 0;
}

// 254 BLZSFileDdaBegin
int __stdcall BLZSFileDdaBegin(HANDLE hFile, BLZUNKNOWN a2, BLZUNKNOWN a3)
{
	return 0;
}

// 255 BLZSFileDdaBeginEx
int __stdcall BLZSFileDdaBeginEx(HANDLE hFile, BLZUNKNOWN a2, BLZUNKNOWN a3, BLZUNKNOWN lDistanceToMove, BLZUNKNOWN a5, BLZUNKNOWN a6, BLZUNKNOWN a7)
{
	return 0;
}

// 256 BLZSFileDdaDestroy
BOOL __stdcall BLZSFileDdaDestroy()
{
	return 0;
}

// 257 BLZSFileDdaEnd
BOOL __stdcall BLZSFileDdaEnd(HANDLE hDirectSound)
{
	return 0;
}

// 258 BLZSFileDdaGetPos
BOOL __stdcall BLZSFileDdaGetPos(HANDLE hDirectSound, BLZUNKNOWN a2, BLZUNKNOWN a3)
{
	return 0;
}

// 259 BLZSFileDdaGetVolume
BOOL __stdcall BLZSFileDdaGetVolume(HANDLE hDirectSound, BLZUNKNOWN a2, BLZUNKNOWN a3)
{
	return 0;
}

// 260 BLZSFileDdaInitialize
BOOL __stdcall BLZSFileDdaInitialize(HANDLE hDirectSound)
{
	return 0;
}

// 261 BLZSFileDdaSetVolume
BOOL __stdcall BLZSFileDdaSetVolume(HANDLE hDirectSound, signed int bigVolume, signed int volume)
{
	return 0;
}

// 262 BLZSFileDestroy
BOOL __stdcall BLZSFileDestroy()
{
	return 0;
}

// 263 BLZSFileEnableDirectAccess
BOOL __stdcall BLZSFileEnableDirectAccess(HANDLE hFile)
{
	return 0;
}

// 264 BLZSFileGetFileArchive
BOOL __stdcall BLZSFileGetFileArchive(HANDLE hFile, HANDLE hArchive)
{
	return 0;
}

// 265 BLZSFileGetFileSize
DWORD __stdcall BLZSFileGetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
{
	return 0;
}

// 266 BLZSFileOpenArchive
BOOL __stdcall BLZSFileOpenArchive(const char *archiveName, DWORD dwPriority, DWORD dwFlags, HANDLE *pArchiveHandle)
{
	return 0;
}

// 267 BLZSFileOpenFile
BOOL __stdcall BLZSFileOpenFile(const char *fileName, HANDLE *pFileHandle)
{
	return 0;
}

// 268 BLZSFileOpenFileEx
BOOL __stdcall BLZSFileOpenFileEx(HANDLE hArchive, const char *fileName, DWORD searchScope, HANDLE *pFileHandle)
{
	return 0;
}

// 269 BLZSFileReadFile
BOOL __stdcall BLZSFileReadFile(HANDLE hFile, void *pBuffer, DWORD nNumberOfBytesToRead, DWORD *pNumberOfBytesRead, LONG lpDistanceToMoveHigh)
{
	return 0;
}

// 270 BLZSFileSetBasePath
BOOL __stdcall BLZSFileSetBasePath(const char *pathName)
{
	return 0;
}

// 271 BLZSFileSetFilePointer
LONG __stdcall BLZSFileSetFilePointer(HANDLE hFile, LONG filePos, LONG *filePosHigh, DWORD dwMethod)
{
	return 0;
}

// 272 BLZSFileSetLocale
LCID __stdcall BLZSFileSetLocale(LCID lcNewLocale)
{
	return 0;
}

// 273 BLZSFileGetBasePath
BOOL __stdcall BLZSFileGetBasePath(char *pBasePathBuffer, size_t bufferSize)
{
	return 0;
}

// 274 BLZSFileSetIoErrorMode
BOOL __stdcall BLZSFileSetIoErrorMode(int mode1, int mode2)
{
	return 0;
}

// 275 BLZSFileGetArchiveName
BOOL __stdcall BLZSFileGetArchiveName(HANDLE hArchive, char *pNameBuffer, size_t bufferSize)
{
	return 0;
}

// 276 BLZSFileGetFileName
BOOL __stdcall BLZSFileGetFileName(HANDLE hFile, char *pNameBuffer, size_t bufferSize)
{
	return 0;
}

// 277 BLZSFileGetArchiveInfo
BOOL __stdcall BLZSFileGetArchiveInfo(HANDLE hArchive, int *a2, int *a3)
{
	return 0;
}

// 278 BLZSFileSetPlatform
void __stdcall BLZSFileSetPlatform(char platform)
{
}

// 279 BLZSFileLoadFile
BOOL __stdcall BLZSFileLoadFile(const char *filePath, void **ppBuffer, size_t *pSize, size_t extraSizeToAlocate, LPOVERLAPPED pOverlapped)
{
	return 0;
}

// 280 BLZSFileUnloadFile
BOOL __stdcall BLZSFileUnloadFile(void *pBuffer)
{
	return 0;
}

// 281 BLZSFileLoadFileEx
BOOL __stdcall BLZSFileLoadFileEx(HANDLE hArchive, const char *filePath, char **ppBuffer, size_t *pBufferSize, size_t extraSize, DWORD searchScope, LPOVERLAPPED pOverlapped)
{
	return 0;
}

// 282 BLZSFilePrioritizeRequest
void __stdcall BLZSFilePrioritizeRequest(BOOL a1, int priority)
{
}

// 283 BLZSFileCancelRequest
void __stdcall BLZSFileCancelRequest(BLZUNKNOWN a1)
{
}

// 284 BLZSFileSetAsyncBudget
void __stdcall BLZSFileSetAsyncBudget(int asyncBudget)
{
}

// 285 BLZSFileSetDataChunkSize
void __stdcall BLZSFileSetDataChunkSize(int dataChunkSize)
{
}

// 286 BLZSFileEnableSeekOptimization
void __stdcall BLZSFileEnableSeekOptimization(BOOL enable)
{
}

// 287 BLZSFileReadFileEx
// 288 BLZSFileFileExists
BOOL __stdcall BLZSFileFileExists(const char *filePath)
{
	return 0;
}

// 289 BLZSFileFileExistsEx
BOOL __stdcall BLZSFileFileExistsEx(HANDLE hArchive, const char *filePath, DWORD searchScope)
{
	return 0;
}

// 291 BLZSFileReadFileEx2
// 292 BLZSFileLoadFileEx2
BOOL __stdcall BLZSFileLoadFileEx2(HANDLE hArchive, const char *filePath, char **ppBuffer, size_t *pBufferSize, size_t extraSize, DWORD searchScope, LPOVERLAPPED pOverlapped, int a8)
{
	return 0;
}

// 293 BLZSFileOpenFileAsArchive
// 294 BLZSFileGetLocale
LCID __stdcall BLZSFileGetLocale()
{
	return 0;
}

// 295 BLZSFileRegisterLoadNotifyProc
void __stdcall BLZSFileRegisterLoadNotifyProc(TPFFileOpenCallback callback, void *pUserData)
{
}

// 296 BLZSFileGetFileCompressedSize
DWORD __stdcall BLZSFileGetFileCompressedSize(HANDLE hFile, DWORD *pSizeHigh)
{
	return 0;
}

// 297 BLZStorm_297
BOOL __stdcall BLZStorm_297(HANDLE hFile, char *buffer, size_t bufferSize)
{
	return 0;
}

// 298 BLZSFileNull
void __stdcall BLZSFileNull()
{
}
 // Debug function, empty in release version.
// 299 BLZSFileAuthenticateArchiveEx

/* Storm memory functions */
// 400 BLZSMemInit
void __stdcall BLZSMemInit()
{
}

// 401 BLZSMemAlloc
void *__stdcall BLZSMemAlloc(size_t size, const char *sourceName, DWORD sourceLine, DWORD flag)
{
	return 0;
}

// 402 BLZSMemDestroy
// Already removed
BOOL __stdcall BLZSMemDestroy()
{
	return 0;
}

// 403 BLZSMemFree
BOOL __stdcall BLZSMemFree(void *pMemory, const char *sourceName, DWORD sourceLine, DWORD flag)
{
	return 0;
}

// 404 BLZSMemGetSize
int __stdcall BLZSMemGetSize(void *pMemory, const char *sourceName, DWORD sourceLine)
{
	return 0;
}

// 405 BLZSMemReAlloc
void *__stdcall BLZSMemReAlloc(void *pMemory, size_t size, const char *sourceName, DWORD sourceLine, DWORD flag)
{
	return 0;
}

// 406 BLZSMemGetInfo
void *__stdcall BLZSMemGetInfo(int *p1, int *p2, int *p3)
{
	return 0;
}


// 491 BLZSMemCopy
void __stdcall BLZSMemCopy(void *pDest, const void *pSource, size_t size)
{
}


/* Storm error handling */
// 465 BLZSErrSetLastError
void __stdcall BLZSErrSetLastError(DWORD dwErrCode)
{
}


/* Storm string management */
// 500 BLZSStrInit
void __stdcall BLZSStrInit()
{
}

// 501 BLZSStrCopy
char *__stdcall BLZSStrCopy(char *dest, const char *source, size_t sizeLimit)
{
	return 0;
}

// 502 BLZSStrHashCombine
DWORD __stdcall BLZSStrHashCombine(const char *string, DWORD flag, DWORD seed)
{
	return 0;
}

// 503 BLZSStrNCat
int __stdcall BLZSStrNCat(char *base, const char *source, size_t sizeLimit)
{
	return 0;
}

// 504 BLZSStrTokenize
void __stdcall BLZSStrTokenize(const char **string, char **token, char *buffer, const char *whitespace, size_t size)
{
}

// 505 BLZSStr505
char *__stdcall BLZSStr505(const char *string, char c, int type)
{
	return 0;
}
    // BLZSStrPack, BLZSStrHash64, or BLZSStrHashHT
// 506 BLZSStrLen
int __stdcall BLZSStrLen(const char *string)
{
	return 0;
}

// 507 BLZSStrDup
char *__stdcall BLZSStrDup(const char *string)
{
	return 0;
}

// 508 BLZSStrCmp
int __stdcall BLZSStrCmp(const char *string1, const char *string2, size_t size)
{
	return 0;
}

// 509 BLZSStrCmpI
int __stdcall BLZSStrCmpI(const char *string1, const char *string2, size_t size)
{
	return 0;
}

// 510 BLZSStrUpper
char *__stdcall BLZSStrUpper(char *string)
{
	return 0;
}

// 569 BLZSStrChr
char *__stdcall BLZSStrChr(const char *string, char c)
{
	return 0;
}

// 570 BLZSStrChrR
char *__stdcall BLZSStrChrR(const char *string, char c)
{
	return 0;
}

// 573 BLZSStrToDouble
double __stdcall BLZSStrToDouble(const char *string)
{
	return 0;
}

// 574 BLZSStrToFloat
float __stdcall BLZSStrToFloat(const char *string)
{
	return 0;
}

// 575 BLZSStrToInt
signed int __stdcall BLZSStrToInt(const char *string)
{
	return 0;
}

// 576 BLZSStrToUnsigned
unsigned int __stdcall BLZSStrToUnsigned(const char *string)
{
	return 0;
}

// 577 BLZSStrToInt64
__int64 __stdcall BLZSStrToInt64(const char *string)
{
	return 0;
}

// 578 BLZSStrPrintf
int __cdecl BLZSStrPrintf(char *dest, size_t size, const char *format, ...)
{
	return 0;
}

// 579 BLZSStrLower
char *__stdcall BLZSStrLower(char *string)
{
	return 0;
}

// 580 BLZSStr580
int __stdcall BLZSStr580(int a1, int a2, int a3, int a4)
{
	return 0;
}
     // BLZSStrPack, BLZSStrHash64, or BLZSStrHashHT
// 581 BLZSStrVPrintf
int __cdecl BLZSStrVPrintf(char *dest, size_t size, const char *format, va_list vl)
{
	return 0;
}

// 588 BLZSStrStr
const char *__fastcall BLZSStrStr(const char *string1, const char *stringSearch)
{
	return 0;
}
 // Same as 596
// 590 BLZSStrHash
DWORD __stdcall BLZSStrHash(const char *string)
{
	return 0;
}
