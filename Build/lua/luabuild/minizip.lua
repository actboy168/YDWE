local ffi = require 'ffi'
ffi.cdef[[
    typedef unsigned int zipFile;

    /* tm_zip contain date/time info */
    struct tm_zip
    {
        uint32_t tm_sec;            /* seconds after the minute - [0,59] */
        uint32_t tm_min;            /* minutes after the hour - [0,59] */
        uint32_t tm_hour;           /* hours since midnight - [0,23] */
        uint32_t tm_mday;           /* day of the month - [1,31] */
        uint32_t tm_mon;            /* months since January - [0,11] */
        uint32_t tm_year;           /* years - [1980..2044] */
    };

    struct zip_fileinfo
    {
        tm_zip      tmz_date;       /* date in understandable format           */
        uint32_t    dosDate;        /* if dos_date == 0, tmu_date is used      */
        uint32_t    internal_fa;    /* internal file attributes        2 bytes */
        uint32_t    external_fa;    /* external file attributes        4 bytes */
    };

    enum {
        MINIZIP_OK                          = 0,
        MINIZIP_ERRNO                       = -1,
        MINIZIP_END_OF_LIST_OF_FILE         = -100,
        MINIZIP_PARAMERROR                  = -102,
        MINIZIP_BADZIPFILE                  = -103,
        MINIZIP_INTERNALERROR               = -104,
        MINIZIP_CRCERROR                    = -105
    };

    enum {
        APPEND_STATUS_CREATE      = 0,
        APPEND_STATUS_CREATEAFTER = 1,
        APPEND_STATUS_ADDINZIP    = 2,
    };

    zipFile zipOpen64(const char *pathname, int append);
    int zipOpenNewFileInZip4_64(
        zipFile file,
        const char* filename,
        const zip_fileinfo* zipfi,
        const void* extrafield_local, 
        unsigned int size_extrafield_local,
        const void* extrafield_global,
        unsigned int size_extrafield_global,
        const char* comment,
        int method,
        int level,
        int raw,
        int windowBits,
        int memLevel,
        int strategy,
        const char* password,
        unsigned long crcForCrypting,
        unsigned long versionMadeBy,
        unsigned long flagBase,
        int zip64
    );
    int zipWriteInFileInZip(zipFile file, const void* buf, unsigned int len);
    int zipCloseFileInZip(zipFile file);
    int zipClose(zipFile file, const char* global_comment);
]]

local minizip = ffi.load('minizip')

local errors = {
    [minizip.MINIZIP_ERRNO] = 'errno',
    [minizip.MINIZIP_END_OF_LIST_OF_FILE] = 'end of list',
    [minizip.MINIZIP_PARAMERROR] = 'invalid argument',
    [minizip.MINIZIP_BADZIPFILE] = 'bad zip file',
    [minizip.MINIZIP_INTERNALERROR] = 'internal error',
    [minizip.MINIZIP_CRCERROR] = 'crc error',
}

local function check_error(func, ret)
    if ret == 0 then return end
    error(('minizip::%s error %d: %s'):format(func, ret, errors[ret or 'unknown error']))
end

local zip = {}
zip.__index = zip

function zip:add_file(t)
    if type(t) == 'string' then
        t = {filename = t}
    end
    assert(t.filename, 'filename missing')

    local info = ffi.new('zip_fileinfo')
    if t.date then
        info.dosDate = 0
        info.tmz_date.tm_sec   = t.date.sec
        info.tmz_date.tm_min   = t.date.min
        info.tmz_date.tm_hour  = t.date.hour
        info.tmz_date.tm_mday  = t.date.day
        info.tmz_date.tm_mon   = t.date.month - 1
        info.tmz_date.tm_year  = t.date.year
    else
        info.dosDate = t.dosDate
    end
    info.internal_fa = t.internal_fa
    info.external_fa = t.external_fa

    t.method = t.method or 8
    t.level = t.level or 9
    t.memLevel = t.memLevel or 9
    t.windowBits = t.windowBits or 15
    t.strategy = t.strategy or 0
    t.local_extra_size = t.local_extra and (t.local_extra_size or #t.local_extra) or 0
    t.file_extra_size  = t.file_extra  and (t.file_extra_size  or #t.file_extra)  or 0

    check_error('zipOpenNewFileInZip4_64',
        minizip.zipOpenNewFileInZip4_64(self.h, t.filename, info,
            t.local_extra, t.local_extra_size, t.file_extra, t.file_extra_size,
            t.comment, t.method, t.level, t.raw,
            t.windowBits, t.memLevel, t.strategy,
            t.password, t.crc, t.versionMadeBy, t.flagBase, t.zip64))
end

function zip:write(data, sz)
    check_error('zipWriteInFileInZip', minizip.zipWriteInFileInZip(self.h, data, sz or #data))
end

function zip:close_file()
    check_error('zipCloseFileInZip', minizip.zipCloseFileInZip(self.h))
end

function zip:archive(t, s, sz)
    self:add_file(t)
    self:write(s, sz)
    self:close_file()
end

function zip:close(global_comment)
    check_error('zipClose', minizip.zipClose(self.h, global_comment))
end

local function zip_open(filename, append)
    local h = minizip.zipOpen64(filename, append)
    if not h then
        error('minizip::zipOpen64 error.')
    end
    return setmetatable({ h = h }, zip)
end

local function unzip_open()
    error('unzip is not implemented.')
end

return function(filename, mode)
    if not mode or mode == 'r' then
        return unzip_open(filename)
    elseif mode == 'w' then
        return zip_open(filename, minizip.APPEND_STATUS_CREATE)
    elseif mode == 'a' then
        return zip_open(filename, minizip.APPEND_STATUS_ADDINZIP)
    else
        error("invalid mode. nil, 'r', 'w', 'a', expected.")
    end
end
