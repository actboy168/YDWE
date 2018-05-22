local ffi = require 'ffi'
local uni = require 'ffi.unicode'
ffi.cdef[[
    struct SHFILEOPSTRUCTW
    {
        unsigned int    hwnd;
        unsigned int    wFunc;
        const wchar_t*  pFrom;
        const wchar_t*  pTo;
        short           fFlags;
        int             fAnyOperationsAborted;
        void*           hNameMappings;
        const wchar_t*  lpszProgressTitle; // only used if FOF_SIMPLEPROGRESS
    };
    int SHFileOperationW(SHFILEOPSTRUCTW* lpFileOp);
]]

local FO_MOVE   = 0x0001
local FO_COPY   = 0x0002
local FO_DELETE = 0x0003
local FO_RENAME = 0x0004

local FOF_MULTIDESTFILES        = 0x0001
local FOF_CONFIRMMOUSE          = 0x0002
local FOF_SILENT                = 0x0004
local FOF_RENAMEONCOLLISION     = 0x0008
local FOF_NOCONFIRMATION        = 0x0010
local FOF_WANTMAPPINGHANDLE     = 0x0020
local FOF_ALLOWUNDO             = 0x0040
local FOF_FILESONLY             = 0x0080
local FOF_SIMPLEPROGRESS        = 0x0100
local FOF_NOCONFIRMMKDIR        = 0x0200
local FOF_NOERRORUI             = 0x0400
local FOF_NOCOPYSECURITYATTRIBS = 0x0800
local FOF_NORECURSION           = 0x1000
local FOF_NO_CONNECTED_ELEMENTS = 0x2000
local FOF_WANTNUKEWARNING       = 0x4000
local FOF_NORECURSEREPARSE      = 0x8000

local shell32 = ffi.load 'shell32'

function fs.delete(path)
    local wpath = uni.u2w(path:string())
    local op = ffi.new('struct SHFILEOPSTRUCTW')
    op.hwnd   = 0
    op.wFunc  = FO_DELETE
    op.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR 
    op.pFrom  = wpath
    op.pTo    = nil
    op.fAnyOperationsAborted = 0
    op.hNameMappings = 0
    op.lpszProgressTitle = nil
    local res = shell32.SHFileOperationW(op)
end
