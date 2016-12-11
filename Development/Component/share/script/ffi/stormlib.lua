local ffi = require 'ffi'
ffi.cdef[[
	struct SFILE_CREATE_MPQ {
    	unsigned long cbSize;         // Size of this structure, in bytes
    	unsigned long dwMpqVersion;   // Version of the MPQ to be created
    	void*         pvUserData;     // Reserved, must be NULL
    	unsigned long cbUserData;     // Reserved, must be 0
    	unsigned long dwStreamFlags;  // Stream flags for creating the MPQ
    	unsigned long dwFileFlags1;   // File flags for (listfile). 0 = default
    	unsigned long dwFileFlags2;   // File flags for (attributes). 0 = default
    	unsigned long dwFileFlags3;   // File flags for (signature). 0 = default
    	unsigned long dwAttrFlags;    // Flags for the (attributes) file. If 0, no attributes will be created
    	unsigned long dwSectorSize;   // Sector size for compressed files
    	unsigned long dwRawChunkSize; // Size of raw data chunk
    	unsigned long dwMaxFileCount; // File limit for the MPQ
	};

	bool SFileCreateArchive2(const wchar_t* szMpqName, struct SFILE_CREATE_MPQ* pCreateInfo, uint32_t* phMpq);
	bool SFileOpenArchive(const wchar_t* szMpqName, unsigned long dwPriority, unsigned long dwFlags, uint32_t* phMpq);
	bool SFileCloseArchive(uint32_t hMpq);
	bool SFileAddFileEx(uint32_t hMpq, const wchar_t* szFileName, const char* szArchivedName, unsigned long dwFlags, unsigned long dwCompression, unsigned long dwCompressionNext);
	bool SFileExtractFile(uint32_t hMpq, const char* szToExtract, const wchar_t* szExtracted, unsigned long dwSearchScope);
	bool SFileHasFile(uint32_t hMpq, const char* szFileName);
	bool SFileSetMaxFileCount(uint32_t hMpq, unsigned long dwMaxFileCount);
	
	bool SFileCreateFile(uint32_t hMpq, const char* szArchivedName, unsigned long long FileTime, unsigned long dwFileSize, unsigned long lcLocale, unsigned long dwFlags, uint32_t* phFile);
	bool SFileWriteFile(uint32_t hFile, const void* pvData, unsigned long dwSize, unsigned long dwCompression);
	bool SFileFinishFile(uint32_t hFile);
	bool SFileOpenFileEx(uint32_t hMpq, const char* szFileName, unsigned long dwSearchScope, uint32_t* phFile);
	bool SFileReadFile(uint32_t hFile, void* lpBuffer, unsigned long dwToRead, unsigned long* pdwRead, void* lpOverlapped);
	unsigned long SFileGetFileSize(uint32_t hFile, unsigned long* pdwFileSizeHigh);
	bool SFileCloseFile(uint32_t hFile);

	unsigned long SFileGetLocale();

	unsigned long GetLastError();
]]
ffi.cdef[[
    struct SYSTEMTIME {
        unsigned short wYear;
        unsigned short wMonth;
        unsigned short wDayOfWeek;
        unsigned short wDay;
        unsigned short wHour;
        unsigned short wMinute;
        unsigned short wSecond;
        unsigned short wMilliseconds;
    };
    struct FILETIME {
        unsigned long dwLowDateTime;
        unsigned long dwHighDateTime;
    };
    void GetSystemTime(struct SYSTEMTIME* lpSystemTime);
    int SystemTimeToFileTime(const struct SYSTEMTIME* lpSystemTime, struct FILETIME*lpFileTime);
]]

require 'filesystem'
local uni = require 'ffi.unicode'
local stormlib = ffi.load('stormlib')

local function current_filetime()
    local systemtime = ffi.new('struct SYSTEMTIME')
    local filetime = ffi.new('struct FILETIME')
    ffi.C.GetSystemTime(systemtime)
    if not ffi.C.SystemTimeToFileTime(systemtime, filetime) then
        return 0
    end
    return filetime.dwLowDateTime | (filetime.dwHighDateTime << 32)
end


local wfile = {}
wfile.__index = wfile

function wfile:close()
	if self.handle == 0 then
		return
	end
	stormlib.SFileFinishFile(self.handle)
	self.handle = 0
end

function wfile:write(buf)
	if self.handle == 0 then
		return false
	end
	return stormlib.SFileWriteFile(self.handle, buf, #buf, 0x02)
end

local rfile = {}
rfile.__index = rfile

function rfile:close()
	if self.handle == 0 then
		return
	end
	stormlib.SFileCloseFile(self.handle)
	self.handle = 0
end

function rfile:size()
	if self.handle == 0 then
		return 0
	end
	local size_hi = ffi.new('unsigned long[1]', 0)
	local size_lo = stormlib.SFileGetFileSize(self.handle, size_hi)
	return size_lo | (size_hi[0] << 32)
end

function rfile:read(n)
	if self.handle == 0 then
		return nil
	end
	if not n then
		n = self:size()
	end
	local buf = ffi.new('char[?]', n)
	local pread = ffi.new('unsigned long[1]', 0)
	if not stormlib.SFileReadFile(self.handle, buf, n, pread, nil) then
		return nil
	end
	return ffi.string(buf, pread[0])
end

local archive = {}
archive.__index = archive

function archive:close()
	if self.handle == 0 then
		return
	end
	stormlib.SFileCloseArchive(self.handle)
	self.handle = 0
end

function archive:add_file(name, path)
	if self.handle == 0 then
		return false
	end
	local wpath = uni.u2w(path:string())
	return stormlib.SFileAddFileEx(self.handle, wpath, name,
			0x00000200 | 0x80000000, -- MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING,
			0x02, -- MPQ_COMPRESSION_ZLIB,
			0x02 --MPQ_COMPRESSION_ZLIB
			)
end

function archive:extract(name, path)
	if self.handle == 0 then
		return false
	end
	local dir = path:parent_path()
	if not fs.exists(dir) then
		fs.create_directories(dir)
	end
	local wpath = uni.u2w(path:string())
	return stormlib.SFileExtractFile(self.handle, name, wpath,
			0 --SFILE_OPEN_FROM_MPQ
			)
end

function archive:has_file(name)
	if self.handle == 0 then
		return false
	end
	return stormlib.SFileHasFile(self.handle, name)
end

function archive:open_file(name)
	if self.handle == 0 then
		return nil
	end
	local phandle = ffi.new('uint32_t[1]', 0)
	if not stormlib.SFileOpenFileEx(self.handle, name, 0, phandle) then
		return nil
	end
	return setmetatable({ handle = phandle[0] }, rfile)
end

function archive:create_file(name, size, filetime)
	if self.handle == 0 then
		return nil
	end
	if not filetime then
		filetime = current_filetime()
	end
	local phandle = ffi.new('uint32_t[1]', 0)
	if not stormlib.SFileCreateFile(self.handle, name, filetime, size, stormlib.SFileGetLocale(), 0x00000200 | 0x80000000, phandle) then
		return nil
	end
	return setmetatable({ handle = phandle[0] }, wfile)
end

function archive:load_file(name)
	if self.handle == 0 then
		return nil
	end
	local file = self:open_file(name)
	if not file then
		return nil
	end
	local content = file:read()
	file:close()
	return content
end

function archive:save_file(name, buf, filetime)
	if self.handle == 0 then
		return false
	end
	local file = self:create_file(name, #buf, filetime)
	if not file then
		return false
	end
	file:write(buf)
	file:close()
	return true
end

function archive:__pairs()
	local content = self:load_file('(listfile)')
	if not content then
		error('(listfile)导出失败')
		return
	end
	if content:sub(1, 3) == '\xEF\xBB\xBF' then
		content = content:sub(4)
	end
	return content:gmatch '[^\n\r]+'
end

local m = {}
function m.open(path, readonly, filecount)
	local wpath = uni.u2w(path:string())
	local phandle = ffi.new('uint32_t[1]', 0)
	local flag = 0
	if readonly then
		flag = 0x100
	end
	if not stormlib.SFileOpenArchive(wpath, 0, flag, phandle) then
		return nil
	end
	if filecount then
		stormlib.SFileSetMaxFileCount(phandle[0], filecount)
	end
	return setmetatable({ handle = phandle[0] }, archive)
end
function m.create(path, filecount)
	local wpath = uni.u2w(path:string())
	local phandle = ffi.new('uint32_t[1]', 0)
	local info = ffi.new('struct SFILE_CREATE_MPQ')
	info.cbSize = ffi.sizeof('struct SFILE_CREATE_MPQ')
	info.dwMpqVersion   = 0 --MPQ_FORMAT_VERSION_1
	info.pvUserData     = nil
    info.cbUserData     = 0
	info.dwStreamFlags  = 0 --STREAM_PROVIDER_FLAT | BASE_PROVIDER_FILE
	info.dwFileFlags1   = 0x80000000 --MPQ_FILE_EXISTS
	info.dwFileFlags2   = 0x80000000 --MPQ_FILE_EXISTS
	info.dwFileFlags3   = 0x80000000 --MPQ_FILE_EXISTS
	info.dwAttrFlags    = 7 --MPQ_ATTRIBUTE_CRC32 | MPQ_ATTRIBUTE_FILETIME | MPQ_ATTRIBUTE_MD5
	info.dwSectorSize   = 0x10000
	info.dwRawChunkSize = 0
	info.dwMaxFileCount = filecount
	if not stormlib.SFileCreateArchive2(wpath, info, phandle) then
		return nil
	end
	return setmetatable({ handle = phandle[0] }, archive)
end
function m.attach(handle)
	return setmetatable({ handle = handle }, archive)
end
return m
