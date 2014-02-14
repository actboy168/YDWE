# -*- coding: utf8 -*-
 
import os
import filesystem as fs
import stormlib

def add_directory(mpq, from_path, from_name = fs.path('')):
    src_path = from_path / from_name
    for name in os.listdir(str(src_path)):
        file_path = src_path / name
        if fs.is_directory(file_path):
            add_directory(mpq, from_path, from_name / name)
        else:
            try:
                mpq.AddFileEx(str(file_path).encode('ascii'), str(from_name / name).encode('ascii'))
            except:
                pass
            
def create(stormlib_path, from_path, to_path):
    module = stormlib.Open(str(stormlib_path))
    fs.remove(to_path)
    fs.create_directories(to_path.parent_path())
    mpq = module.Open(str(to_path).encode('ascii'))
    if mpq is None:
        raise Exception('Can\'t open mpq ' + str(to_path))
    add_directory(mpq, from_path)
    if not mpq.Compact():
        raise Exception('Mpq compact failed.')
        
    del mpq
