local ffi = require 'ffi'
ffi.cdef[[
	uint32_t LoadLibraryA(const char* lpFileName);
    typedef void (*ffi_anyfunc)();
	ffi_anyfunc GetProcAddress(uint32_t hModule, uint32_t ord);

    typedef bool (*__stdcall SFileOpenArchive)(const char* szArchivename, unsigned long dwPriority, unsigned long dwFlags, uint32_t* handle);
    typedef bool (*__stdcall SFileLoadFile)(const char* filename, const void** pbuf, uint32_t* plen, uint32_t reservelen, void* overlapped_ptr);
    typedef bool (*__stdcall SFileUnloadFile)(const void* buf);
    typedef bool (*__stdcall SFileExists)(const char* name);
    typedef uint32_t (*__stdcall SStrHash)(const char* str);
]]

local uni = require 'ffi.unicode'
local storm = ffi.C.LoadLibraryA('storm.dll')
local SFileOpenArchive = ffi.cast('SFileOpenArchive', ffi.C.GetProcAddress(storm, 266))
local SFileLoadFile = ffi.cast('SFileLoadFile', ffi.C.GetProcAddress(storm, 279))
local SFileUnloadFile = ffi.cast('SFileUnloadFile', ffi.C.GetProcAddress(storm, 280))
local SFileExists = ffi.cast('SFileExists', ffi.C.GetProcAddress(storm, 288))
local SStrHash = ffi.cast('SStrHash', ffi.C.GetProcAddress(storm, 590))

local storm = {}

function storm.open(path, priority)
	local apath = uni.u2a(path:string())
	local phandle = ffi.new('uint32_t[1]', 0)
    if not SFileOpenArchive(apath, priority, 0, phandle) then
        return nil
    end
    return phandle[0]
end

function storm.extract_file(path, name)
	local aname = uni.u2a(name)
	local pbuf = ffi.new('void*[1]', 0)
	local plen = ffi.new('uint32_t[1]', 0)
    if not SFileLoadFile(aname, pbuf, plen, 0, nil) then
        return false
    end
    local buf = ffi.string(pbuf[0], plen[0])
    SFileUnloadFile(pbuf[0])
    return io.save(path, buf)
end

function storm.load_file(name)
	local aname = uni.u2a(name)
	local pbuf = ffi.new('void*[1]', 0)
	local plen = ffi.new('uint32_t[1]', 0)
    if not SFileLoadFile(aname, pbuf, plen, 0, nil) then
        return false
    end
    local buf = ffi.string(pbuf[0], plen[0])
    SFileUnloadFile(pbuf[0])
    return buf
end

function storm.has_file(name)
    return SFileExists(name)
end

function storm.string_hash(str)
    return SStrHash(str)
end

return storm
