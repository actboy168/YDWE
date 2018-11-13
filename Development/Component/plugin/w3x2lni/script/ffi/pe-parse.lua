local DataDirName = {
    'ExportTable',
    'ImportTable',
    'ResourceTable',
    'ExceptionTable',
    'CertificateTable',
    'BaseRelocationTable',
    'Debug',
    'Architecture',
    'GlobalPtr',
    'TLSTable',
    'LoadConfigTable',
    'BoundImport',
    'IAT',
    'DelayImportDescriptor',
    'CLRRuntimeHeader',
    'Reserved'
}

local function readlist(f, list, t)
    t = t or {}
    for _, c in ipairs(list) do
        local name, size = c[1], c[2]
        assert(t[name] == nil, 'Value for \''..name..'\' already set')
        t[name] = ('I' .. size):unpack(f:read(size))
    end
    return t
end

local function fileoffset(f, obj, rva)
    if f.type == 'memory' then
        return rva
    end
    for _, s in ipairs(obj.Sections) do
        if s.VirtualAddress <= rva and s.VirtualAddress + s.VirtualSize >= rva then
            return rva - s.VirtualAddress + s.PointerToRawData
        end
    end
    return nil, 'No match RVA with Section list, RVA out of bounds'
end

local function readstring(f)
    local name = ''
    while true do
        local c = f:read(1)
        if c == '\0' then break end
        name = name .. c
    end
    return name
end

local function importtable(f, obj)
    local ImportTable = obj.DataDirectory.ImportTable
    f:seek(assert(fileoffset(f, obj, ImportTable.VirtualAddress)))
    while true do
        local dll = readlist(f, {
            { 'ImportLookupTableRVA', 4 },
            { 'TimeDateStamp', 4 },
            { 'ForwarderChain', 4 },
            { 'NameRVA', 4 },
            { 'ImportAddressTableRVA', 4 },
        })
        if dll.NameRVA == 0 then
            break
        else
            table.insert(ImportTable, dll)
        end
    end
    for _, dll in ipairs(ImportTable) do
        f:seek(assert(fileoffset(f, obj, dll.NameRVA)))
        dll.Name = readstring(f)
    end
end

return function (f)
    local MZ = f:read(2)
    if MZ ~= 'MZ' then
        f:close()
        return nil, 'Not a valid image'
    end
    f:seek(60)
    local peoffset = ('I4'):unpack(f:read(4))
    f:seek(peoffset)
    local peheader = ('z'):unpack(f:read(4) .. '\0')
    if peheader ~= 'PE' then
        f:close()
        return nil, 'Invalid PE header'
    end
    local obj = readlist(f, {
        { 'Machine', 2 },
        { 'NumberOfSections', 2 },
        { 'TimeDateStamp', 4 },
        { 'PointerToSymbolTable', 4 },
        { 'NumberOfSymbols', 4 },
        { 'SizeOfOptionalHeader', 2 },
        { 'Characteristics', 2 },
    })
    
    if obj.SizeOfOptionalHeader > 0 then
        readlist(f, {
            { 'Magic', 2 },
            { 'MajorLinkerVersion', 1 },
            { 'MinorLinkerVersion', 1 },
            { 'SizeOfCode', 4 },
            { 'SizeOfInitializedData', 4 },
            { 'SizeOfUninitializedData', 4 },
            { 'AddressOfEntryPoint', 4 },
            { 'BaseOfCode', 4 },
        }, obj)
        local ptrsize
        if obj.Magic == 0x20b then
            ptrsize = 8
        else
            ptrsize = 4
            readlist(f, {
                { 'BaseOfData', 4 },
            }, obj)
        end
        readlist(f, {
            { 'ImageBase', ptrsize },
            { 'SectionAlignment', 4 },
            { 'FileAlignment', 4 },
            { 'MajorOperatingSystemVersion', 2 },
            { 'MinorOperatingSystemVersion', 2 },
            { 'MajorImageVersion', 2 },
            { 'MinorImageVersion', 2 },
            { 'MajorSubsystemVersion', 2 },
            { 'MinorSubsystemVersion', 2 },
            { 'Win32VersionValue', 4 },
            { 'SizeOfImage', 4 },
            { 'SizeOfHeaders', 4 },
            { 'CheckSum', 4 },
            { 'Subsystem', 2 },
            { 'DllCharacteristics', 2 },
            { 'SizeOfStackReserve', ptrsize },
            { 'SizeOfStackCommit', ptrsize },
            { 'SizeOfHeapReserve', ptrsize },
            { 'SizeOfHeapCommit', ptrsize },
            { 'LoaderFlags', 4 },
            { 'NumberOfRvaAndSizes', 4 },
        }, obj)
        obj.DataDirectory = {}
        for i = 1, obj.NumberOfRvaAndSizes do
            local r = readlist(f, {
                { 'VirtualAddress', 4 },
                { 'Size', 4 },
            })
            r.name = DataDirName[i]
            r.index = i
            obj.DataDirectory[r.name] = r
        end
    end

    obj.Sections = {}
    for i = 1, obj.NumberOfSections do
        local name =  ('z'):unpack(f:read(8) .. '\0')
        obj.Sections[i] = readlist(f, {
            { 'VirtualSize', 4 },
            { 'VirtualAddress', 4 },
            { 'SizeOfRawData', 4 },
            { 'PointerToRawData', 4 },
            { 'PointerToRelocations', 4 },
            { 'PointerToLinenumbers', 4 },
            { 'NumberOfRelocations', 2 },
            { 'NumberOfLinenumbers', 2 },
            { 'Characteristics', 4 },
        })
        obj.Sections[i].Name = name
    end
    importtable(f, obj)
    f:close()
    return obj
end
