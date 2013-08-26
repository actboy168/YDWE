# -*- coding: utf-8 -*-

import util.msvc
import util.path
import util.filesystem as fs
path = util.path.path

def complie(configuration):
    print('complie')
    print('msvc.setup_env')
    util.msvc.setup_env()
    print('msvc.rebuild')
    util.msvc.rebuild(path['BuildRoot']/ 'Editor' / 'Core' / 'Solution' / 'YDWE.sln', configuration)    
    util.msvc.rebuild(path['BuildRoot']/ 'Editor' / 'Plugin' / 'YDColorizer' / 'YDColorizer.sln', configuration, 'Any CPU')
    util.msvc.rebuild(path['BuildRoot']/ 'Editor' / 'UI' / 'sln' / 'YDUI.sln', 'YDWE')
    util.msvc.rebuild(path['BuildRoot']/ 'Editor' / 'UI' / 'sln' / 'YDUI.sln', 'YDTrigger')


def Configuration():
    import sys
    if len(sys.argv) > 1:
        return sys.argv[1]
    return 'Debug'

if __name__ == '__main__':
    util.path.ResetPath(Configuration())
    complie(Configuration())
