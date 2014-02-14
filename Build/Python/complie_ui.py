# -*- coding: utf-8 -*-

import util.msvc
import util.path
import util.filesystem as fs
path = util.path.path

def complie(configuration):
    from pack_ui            import pack_ui
    print('complie_ui')
    util.msvc.setup_env()
    util.msvc.rebuild(path['BuildRoot']/ 'Editor' / 'UI' / 'sln' / 'YDUI.sln', 'YDWE')
    util.msvc.rebuild(path['BuildRoot']/ 'Editor' / 'UI' / 'sln' / 'YDUI.sln', 'YDTrigger')
    pack_ui('ydtrigger')
    pack_ui('ydwe')

def Configuration():
    import sys
    if len(sys.argv) > 1:
        return sys.argv[1]
    return 'Release'

if __name__ == '__main__':
    util.path.ResetPath(Configuration())
    complie(Configuration())
