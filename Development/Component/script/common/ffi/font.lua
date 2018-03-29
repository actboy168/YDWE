local ffi = require 'ffi'
local uni = require 'ffi.unicode'
ffi.cdef [[
    int CreateFontW(int nHeight, int nWidth, int nEscapement, int nOrientation, int fnWeight, unsigned int fdwItalic, unsigned int fdwUnderline, unsigned int fdwStrikeOut, unsigned int fdwCharSet, unsigned int fdwOutputPrecision, unsigned int fdwClipPrecision, unsigned int fdwQuality, unsigned int fdwPitchAndFamily, const wchar_t* lpszFace);

    int GetDC(int hWnd);
    int ReleaseDC(int hWnd, int hdc);
    int GetDeviceCaps(int hdc, int index);
    int MulDiv(int nNumber, int nNumerator, int nDenominator);
]]

local function size_to_height(size)
    local screen_dc = ffi.C.GetDC(0)
    local r = ffi.C.MulDiv(size, ffi.C.GetDeviceCaps(screen_dc, 90), 72)
    ffi.C.ReleaseDC(0, screen_dc)
    return r
end

return function (name, size)
    local wname = uni.u2w(name)
    return ffi.C.CreateFontW(size_to_height(size), 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, wname)
end
