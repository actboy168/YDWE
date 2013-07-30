# -*- coding: utf8 -*-

import sys
import util.filesystem as fs
import util.mpqutil
import util.path

path = util.path.path

def pack_units():
    stormlib_path = path['OpenSource'] / 'StormLib' / 'Current' / 'bin' / 'Win32' / 'Release' / 'StormLib.dll'
    from_path     = path['BuildRoot']  / 'Editor' / 'UI' / 'src' / 'Units'
    to_path       = path['Result']     / 'share' / 'mpq' / 'units.mpq'
    util.mpqutil.create(stormlib_path, from_path, to_path)
    
def Configuration():
    if len(sys.argv) > 1:
        return sys.argv[1]
    return 'Debug'

if __name__ == '__main__':
    util.path.ResetPath(Configuration())
    pack_units()
