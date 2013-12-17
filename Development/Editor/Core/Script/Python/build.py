# -*- coding: utf-8 -*-

import sys
import datetime
import util.filesystem as fs
import util.path

path = util.path.path

class RedirectLog:
    def __init__(self, log, stdout): 
        self.__stdout  = stdout
        self.__logfile = file(log, 'w')
        
    def __del__(self):
        self.__logfile.close()
        
    def write(self, s):
        self.__logfile.write(s)
        self.__stdout.write(s)

def Log():
    logname = 'build.log'
    if len(sys.argv) > 2:
        logname = sys.argv[2]
    sys.stdout = RedirectLog(logname, sys.stdout)
    
def build_clear(configuration):
    print ('build_clear') 
    fs.remove_all(path['ResultRoot'] / 'bin' / configuration)
    fs.remove_all(path['ResultRoot'] / 'obj' / configuration)
    fs.remove_all(path['ResultRoot'] / 'lib' / configuration)
    fs.remove_all(path['ResultRoot'] / 'publish' / configuration)
    fs.remove_all(path['BuildRoot'] / 'UI' / 'bin')
    fs.remove_all(path['BuildRoot'] / 'UI' / 'out')
    fs.remove_all(path['BuildRoot'] / 'UI' / 'txt')

def build_move(configuration):
    print ('build_move')
    fs.copy_directory(
          path['ResultRoot'] / 'bin' / configuration
        , path['ResultRoot'] / 'publish' / configuration
        , ['.pdb', '.exp', '.ilk', '.aps', '.lib']
        , False)
    
def build_all(configuration):
    from complie            import complie
    from build_anti_bj_leak import build_anti_bj_leak
    from copy_all           import copy_all
    from pack_skin          import pack_skin
    from pack_ui            import pack_ui
    from pack_units         import pack_units

    complie(configuration)
    build_anti_bj_leak()
    copy_all(configuration)
    pack_skin()
    pack_ui('ydtrigger')
    pack_ui('ydwe')
    pack_units()

def build(configuration):
    Log()
    print('build')
    t = datetime.datetime.now()
    print ('build {0}'.format(t))
    build_clear(configuration)
    build_all(configuration)
    build_move(configuration)
    fs.remove_all(path['Result'] / 'plugin' / 'YDColorizer')
    print ('time {0}'.format(datetime.datetime.now() - t))
    
def Configuration():
    if len(sys.argv) > 1:
        return sys.argv[1]
    return 'Release'

if __name__ == '__main__':
    util.path.ResetPath(Configuration())
    build(Configuration())
