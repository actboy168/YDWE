local ffi = require 'ffi'
local uni = require 'ffi.unicode'

ffi.cdef [[
    unsigned short __stdcall GetUserDefaultLangID();
    unsigned long  __stdcall GetFileVersionInfoSizeW(const wchar_t* lptstrFilename, unsigned long* lpdwHandle);
    int            __stdcall GetFileVersionInfoW(const wchar_t* lptstrFilename, unsigned long dwHandle, unsigned long dwLen, void* lpData);
    int            __stdcall VerQueryValueW(const void* pBlock, const wchar_t* lpSubBlock, void* lplpBuffer, unsigned int* puLen);
    
    struct VS_FIXEDFILEINFO
    {
        unsigned long   dwSignature;            /* e.g. 0xfeef04bd */
        unsigned long   dwStrucVersion;         /* e.g. 0x00000042 = "0.42" */
        unsigned long   dwFileVersionMS;        /* e.g. 0x00030075 = "3.75" */
        unsigned long   dwFileVersionLS;        /* e.g. 0x00000031 = "0.31" */
        unsigned long   dwProductVersionMS;     /* e.g. 0x00030010 = "3.10" */
        unsigned long   dwProductVersionLS;     /* e.g. 0x00000031 = "0.31" */
        unsigned long   dwFileFlagsMask;        /* = 0x3F for version "0.42" */
        unsigned long   dwFileFlags;            /* e.g. VFF_DEBUG | VFF_PRERELEASE */
        unsigned long   dwFileOS;               /* e.g. VOS_DOS_WINDOWS16 */
        unsigned long   dwFileType;             /* e.g. VFT_DRIVER */
        unsigned long   dwFileSubtype;          /* e.g. VFT2_DRV_KEYBOARD */
        unsigned long   dwFileDateMS;           /* e.g. 0 */
        unsigned long   dwFileDateLS;           /* e.g. 0 */
    };

    struct TRANSLATION {
        unsigned short language;
        unsigned short codepage;
    };
]]

local def_language = ffi.C.GetUserDefaultLangID()
local def_codepage = 1252
local VS_FFI_SIGNATURE = 0xFEEF04BD

local ver = ffi.load 'version.dll'

local function u2w(str)
    local wstr = uni.u2w(str)
    return wstr
end

local function query(data, key, type)
    local info = ffi.new('void*[1]', 0)
    local size = ffi.new('unsigned int[1]')
    if 0 == ver.VerQueryValueW(data, u2w(key), info, size) then
        return
    end
    if size[0] < ffi.sizeof('struct ' .. type) then
        return
    end
    return ffi.cast('struct ' .. type .. '[1]', info[0])[0]
end


local function query_string(data, language, codepage, key)
    local info = ffi.new('void*[1]', 0)
    local size = ffi.new('unsigned int[1]', 0)
    local querykey = ([[\StringFileInfo\%04x%04x\%s]]):format(language, codepage, key)
    if 0 == ver.VerQueryValueW(data, u2w(querykey), info, size) then
        return
    end
    return uni.w2u(ffi.cast('wchar_t*', info[0]), size[0])
end

local mt = {}

function mt:__index(key)
    local res = query_string(self.data, self.language, self.codepage, key)
    if res then
        return res
    end
    local res = query_string(self.data, def_language, self.codepage, key)
    if res then
        return res
    end
    local res = query_string(self.data, self.language, def_codepage, key)
    if res then
        return res
    end
    local res = query_string(self.data, def_language, def_codepage, key)
    if res then
        return res
    end
    return ''
end

local function file_version(fname)
    local self = {}
    local wstr = u2w(fname)
    local size = ver.GetFileVersionInfoSizeW(wstr, nil)
    if size <= 0 then
        return
    end
    local data = ffi.new('unsigned char[?]', size)
    if 0 == ver.GetFileVersionInfoW(wstr, 0, size, data) then
        return
    end
    local info = query(data, [[\]], 'VS_FIXEDFILEINFO')
    if not info then
        return
    end
    if VS_FFI_SIGNATURE ~= info.dwSignature then
        return
    end
    local trans = query(data, [[\VarFileInfo\Translation]], 'TRANSLATION')
    self.data = data
    self.info = info
    if trans then
        self.language = trans.language
        self.codepage = trans.codepage
    else
        self.language = def_language
        self.codepage = def_codepage
    end
    return setmetatable(self, mt)
end

local function split(str, p)
    local r = {}
    str:gsub('[^%' .. p .. ']+', function (w) r[#r+1] = w end)
    return r
end

local function tonum(s)
    if not s then return 0 end
    return tonumber(s) or 0
end

return function (fname)
    local res = {}
    local fv = file_version(fname)
    if fv then
        local str = fv.FileVersion
        if str ~= '' then
            res = split(str, ',')
            if #res < 2 then
                res = split(str, '.')
            end
        end
    end
    return {
        major = tonum(res[1]),
        minor = tonum(res[2]),
        revision = tonum(res[3]),
        build = tonum(res[4]),
    }
end
