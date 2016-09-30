local ffi = require("ffi")

ar.sfmpq = {}

ffi.cdef[[
	void*     MpqOpenArchiveForUpdate(const char* lpFileName, uint32_t dwFlags, uint32_t dwMaximumFilesInArchive);
	bool      MpqCompactArchive(void* hMPQ);
	uint32_t  MpqCloseUpdatedArchive(void* hMPQ, uint32_t dwUnknown2);
	bool      MpqAddFileToArchiveEx(void* hMPQ, const char* lpSourceFileName, const char* lpDestFileName, uint32_t dwFlags, uint32_t dwCompressionType, uint32_t dwCompressLevel);
]]

ar.sfmpq.lib = ffi.load("sfmpq")
ar.sfmpq.MpqOpenArchiveForUpdate = ar.sfmpq.lib.MpqOpenArchiveForUpdate
ar.sfmpq.MpqCompactArchive       = ar.sfmpq.lib.MpqCompactArchive
ar.sfmpq.MpqCloseUpdatedArchive  = ar.sfmpq.lib.MpqCloseUpdatedArchive
ar.sfmpq.MpqAddFileToArchiveEx   = ar.sfmpq.lib.MpqAddFileToArchiveEx
