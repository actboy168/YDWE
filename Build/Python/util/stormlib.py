import ctypes
import struct

MPQ_FORMAT_VERSION_1     = 0x00000000
MPQ_CREATE_ARCHIVE_V1    = 0x00000000

MPQ_FILE_COMPRESS        = 0x00000200
MPQ_FILE_ENCRYPTED       = 0x00010000
MPQ_FILE_REPLACEEXISTING = 0x80000000

BASE_PROVIDER_FILE       = 0x00000000
STREAM_PROVIDER_LINEAR   = 0x00000000

MPQ_COMPRESSION_HUFFMANN     = 0x01
MPQ_COMPRESSION_ZLIB         = 0x02
MPQ_COMPRESSION_LZMA         = 0x12
MPQ_COMPRESSION_SPARSE       = 0x20
MPQ_COMPRESSION_ADPCM_MONO   = 0x40
MPQ_COMPRESSION_ADPCM_STEREO = 0x80


class _MODULE:
    def __init__(self, path):
        self.module = ctypes.windll.LoadLibrary(path)      
        self.SFileCreateArchive  = self.module['SFileCreateArchive']
        self.SFileCreateArchive2 = self.module['SFileCreateArchive2']
        self.SFileOpenArchive    = self.module['SFileOpenArchive']
        self.SFileCloseArchive   = self.module['SFileCloseArchive']
        self.SFileCompactArchive = self.module['SFileCompactArchive']
        self.SFileAddFile        = self.module['SFileAddFile']
        self.SFileAddFileEx      = self.module['SFileAddFileEx']
        
    def CreateArchive(self, path, maxcount, flags):
        mpq = ctypes.c_ulong()
        if (self.SFileCreateArchive(path, flags, maxcount, ctypes.byref(mpq))):
            return mpq.value
        else:
            return None    

    def CreateArchive2(self, path, maxcount, sector_size):
        create_info = struct.pack('<11I'
                                  , 44
                                  , MPQ_FORMAT_VERSION_1
                                  , 0
                                  , 0
                                  , STREAM_PROVIDER_LINEAR | BASE_PROVIDER_FILE
                                  , 0
                                  , 0
                                  , 0
                                  , sector_size
                                  , 0
                                  , maxcount
                                  )
        
        mpq = ctypes.c_ulong()
        if (self.SFileCreateArchive2(path, create_info, ctypes.byref(mpq))):
            return mpq.value
        else:
            return None
        
    def OpenArchive(self, path, flags, file_limit):
        mpq = ctypes.c_ulong()
        if (self.SFileOpenArchive(path, flags, file_limit, ctypes.byref(mpq))):
            return mpq.value
        else:
            return None
        
    def CloseArchive(self, mpq):
        self.SFileCloseArchive(mpq)
    
    def CompactArchive(self, mpq, listfile, reserved):
        return self.SFileCompactArchive(mpq, listfile, reserved)
    
    def AddFile(self, mpq, path, name, flags):
        return self.SFileAddFile(mpq, path, name, flags)
    
    def AddFileEx(self, mpq, path, name, flags, compression, compression_next):
        return self.SFileAddFileEx(mpq, path, name, flags, compression, compression_next)

class _MPQ:
    def __init__(self, module, mpq):
        self.__module = module
        self.__mpq = mpq
        
    def __del__(self):
        self.__module.CloseArchive(self.__mpq)

    def AddFile(self, path, name, flags = MPQ_FILE_REPLACEEXISTING):
        return self.__module.AddFile(self.__mpq, path, name, flags)
    
    def AddFileEx(self, path, name, flags = MPQ_FILE_REPLACEEXISTING | MPQ_FILE_COMPRESS):
        return self.__module.AddFileEx(self.__mpq, path, name, flags, MPQ_COMPRESSION_ZLIB, MPQ_COMPRESSION_ZLIB)

    def Compact(self):
        return self.__module.CompactArchive(self.__mpq, 0, 0)
    
class _STORMLIB:
    def __init__(self, path):
        self.__module = _MODULE(path)
        
    def CreateArchive(self, path, maxcount = 64, flags = MPQ_CREATE_ARCHIVE_V1):
        mpq = self.__module.CreateArchive(path, maxcount, flags)
        if mpq is None:
            return None
        return _MPQ(self.__module, mpq)
    
    def OpenArchive(self, path, priority = 0, flags = 0):
        mpq = self.__module.OpenArchive(path, priority, flags)
        if mpq is None:
            return None
        return _MPQ(self.__module, mpq)
    
    def Open(self, path, maxcount = 64):
        mpq = self.__module.OpenArchive(path, 0, 0)
        if mpq is None:
            mpq = self.__module.CreateArchive2(path, maxcount, 0x10000)
            if mpq is None:
                return None
        return _MPQ(self.__module, mpq)
    
def Open(path):
    return _STORMLIB(path)

