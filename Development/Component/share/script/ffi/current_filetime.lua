local ffi = require 'ffi'
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

return function ()
    local systemtime = ffi.new('struct SYSTEMTIME')
    local filetime = ffi.new('struct FILETIME')
    ffi.C.GetSystemTime(systemtime)
    if not ffi.C.SystemTimeToFileTime(systemtime, filetime) then
        return 0
    end
    return filetime.dwLowDateTime | (filetime.dwHighDateTime << 32)
end
