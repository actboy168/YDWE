# -*- coding: utf8 -*-

import os
import stat
import shutil
import types

def _to_string(p):
    if type(p) is types.StringType:
        return p
    return p.str

class path:
    def __init__(self, p):
            self.str = _to_string(p)
        
    def __div__(self, p):
        return path(os.path.join(self.str, _to_string(p)))
 
    def __str__(self):
        return self.str
    
    def __eq__(self, p):
        return self.str == _to_string(p)

    def parent_path(self):
        return path(os.path.dirname(self.str))

    def remove_filename(self):
        self.str = os.path.dirname(self.str)
        return self

    def filename(self):
        return path(os.path.basename(self.str))
    
    def extension(self):
        return path(os.path.splitext(self.str)[1])

    def replace_extension(self, p):
        self.str = os.path.splitext(self.str)[0] + _to_string(p)
        return self
    
def exists(p):
    return os.path.exists(_to_string(p))

def remove(p):    
    try:
        os.remove(_to_string(p)) 
    except:
        pass
    
def remove_all(p):    
    try:
        shutil.rmtree(_to_string(p)) 
    except:
        pass
    
def create_directories(p):
    try:
        os.makedirs(_to_string(p))
    except WindowsError:
        pass
    
def create_directory(p):
    try:
        os.makedir(_to_string(p))
    except WindowsError:
        pass
    
def is_directory(p):
    return os.path.isdir(_to_string(p))

def copy_file(from_p, to_p):
    from_s = _to_string(from_p)
    to_s   = _to_string(to_p)
    try:
        os.chmod(to_s, stat.S_IWRITE)
    except WindowsError:
        pass
    shutil.copy(from_s, to_s)
    
def copy_directory(from_p, to_p, ext_filter = None, flag = True): 
    create_directories(to_p)
    for name in os.listdir(_to_string(from_p)):
        from_file = path(from_p) / name
        to_file   = path(to_p) / name
        if is_directory(from_file):
            copy_directory(from_file, to_file, ext_filter, flag)
        else:
            if ext_filter is not None:
                ext = str(from_file.extension()).lower()
                if not (flag ^ (ext in ext_filter)):
                    continue
            copy_file(from_file, to_file)
