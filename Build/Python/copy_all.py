# -*- coding: utf8 -*-

import re
import sys
import util.path
import util.filesystem as fs

path = util.path.path

def get_boost_version():
    f = open(str(path['OpenSource'] / 'Boost' / 'boost' / 'version.hpp'))
    try:
        m = re.search(r'\#define[\s]+BOOST_LIB_VERSION[\s]+\"([0-9_]+)\"', f.read())
        if m:
            return m.group(1)
    finally:
        f.close()
    raise

def copy_boost_dll(name, msvc_version, configuration):
    if configuration == 'Release':
        filename = 'boost_' + name + '-vc' + str(msvc_version) + '-mt-' + get_boost_version() + '.dll'
    else:
        filename = 'boost_' + name + '-vc' + str(msvc_version) + '-mt-gd-' + get_boost_version() + '.dll'
    fs.copy_file(path['OpenSource'] / 'Boost' / 'stage' / 'lib' / filename, path['ResultCore'] / filename)
    
def copy_lib_dll(name, configuration, version = 'Current'):
    fs.copy_directory(path['OpenSource'] / name / version / 'bin' / 'Win32' / configuration, path['ResultCore'], ['.dll'])
    
def copy_lua_script():
    fs.copy_directory(path['Development'] / 'Core' / 'Script' / 'lua', path['Result'] / 'share' / 'script', ['.lua'])

def copy_boost_preprocessor():
    fs.copy_directory(path['OpenSource'] / 'Boost' / 'boost' / 'preprocessor', path['ResultPlugin'] / 'wave' / 'include' / 'boost' / 'preprocessor')
    fs.copy_file(path['OpenSource'] / 'Boost' / 'boost' / 'preprocessor.hpp', path['ResultPlugin'] / 'wave' / 'include' / 'boost' / 'preprocessor.hpp')
    
def copy_component():
    fs.copy_directory(path['Development'] / 'Component', path['Result'])

def copy_all(msvc_version, configuration):
    print('copy_all')
    fs.create_directories(path['ResultCore'])
    fs.create_directories(path['ResultCore'] / 'modules')
    copy_boost_dll('system',     msvc_version, configuration)
    copy_boost_dll('filesystem', msvc_version, configuration)
    copy_lib_dll('Lua',          configuration)
    copy_lib_dll('StormLib',     configuration)
    fs.copy_file(path['OpenSource'] / 'sfmpq' / 'bin' / configuration / 'sfmpq.dll', path['ResultPlugin'] / 'jasshelper' / 'sfmpq.dll')
    fs.copy_file(path['OpenSource'] / 'sfmpq' / 'bin' / configuration / 'sfmpq.dll', path['ResultPlugin'] / 'jasshelper' / 'bin' / 'sfmpq.dll')
    fs.copy_file(path['OpenSource'] / 'luaffi' / 'bin' / configuration / 'ffi.dll', path['ResultCore'] / 'modules' / 'ffi.dll')
    copy_boost_preprocessor()
    copy_component()

def Configuration():
    if len(sys.argv) > 1:
        return sys.argv[1]
    return 'Debug'

if __name__ == '__main__':
    util.path.ResetPath(Configuration())
    copy_all(120, Configuration())

