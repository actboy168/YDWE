# -*- coding: utf8 -*-

import sys
import util.path
import util.filesystem as fs

path = util.path.path
    
def copy_crt_dll():
    fs.copy_directory(path['ThirdParty'] / 'Microsoft' / 'CRT' / 'Win32' / 'Microsoft.VC100.CRT', path['ResultCore'], ['.dll'])

def copy_boost_dll(name, configuration):
    if configuration == 'Release':
        filename = 'boost_' + name + '-vc100-mt-1_53.dll'
    else:
        filename = 'boost_' + name + '-vc100-mt-gd-1_53.dll'
    fs.copy_file(path['OpenSource'] / 'Boost' / '1.53' / 'stage' / 'lib' / 'Win32' / filename, path['ResultCore'] / filename)
    
def copy_lib_dll(name, configuration, version = 'Current'):
    fs.copy_directory(path['OpenSource'] / name / version / 'bin' / 'Win32' / configuration, path['ResultCore'], ['.dll'])
    
def copy_lua_script():
    fs.copy_directory(path['BuildRoot'] / 'Editor' / 'Core' / 'Script' / 'lua', path['Result'] / 'share' / 'script', ['.lua'])
    
def copy_log_script():
    fs.create_directories(path['Result'] / 'logs')
    fs.copy_file(path['BuildRoot'] / 'Editor' / 'Core' / 'Script' / 'YDLogger.cfg', path['Result'] / 'logs' / 'YDLogger.cfg')

def copy_boost_preprocessor():
    fs.copy_directory(path['OpenSource'] / 'Boost' / '1.53' / 'boost' / 'preprocessor', path['ResultPlugin'] / 'wave' / 'include' / 'boost' / 'preprocessor')
    fs.copy_file(path['OpenSource'] / 'Boost' / '1.53' / 'boost' / 'preprocessor.hpp',  path['ResultPlugin'] / 'wave' / 'include' / 'boost' / 'preprocessor.hpp')
    
def copy_component():
    fs.copy_directory(path['BuildRoot'] / 'Editor' / 'Component', path['Result'])

def copy_all(configuration):
    if configuration == 'Release':
        copy_crt_dll()
    copy_boost_dll('system',     configuration)
    copy_boost_dll('filesystem', configuration)
    copy_boost_dll('date_time',  configuration)
    copy_boost_dll('thread',     configuration)
    copy_lib_dll('APR',          configuration)
    copy_lib_dll('APR-Util',     configuration)
    copy_lib_dll('Log4CXX',      configuration)
    copy_lib_dll('Zlib',         configuration)
    copy_lib_dll('Lua',          configuration)
    copy_lib_dll('LuaBind',      configuration)
    copy_lib_dll('StormLib',     configuration)
    copy_lib_dll('Zlib',         configuration)
    copy_lua_script()
    copy_log_script()
    copy_boost_preprocessor()
    copy_component()

def Configuration():
    if len(sys.argv) > 1:
        return sys.argv[1]
    return 'Debug'

if __name__ == '__main__':
    print('copy_all')
    util.path.ResetPath(Configuration())
    copy_all(Configuration())

