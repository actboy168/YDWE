# -*- coding: utf8 -*-

import re
import sys
import util.filesystem as fs
import util.mpqutil
import util.path

path = util.path.path

def convert_file(from_ptah, to_path, file_name):
    in_file = open(str(from_ptah / file_name), 'rb')
    out_file = open(str(to_path / fs.path(file_name).replace_extension('.txt')), 'wb')
    reobj = re.compile(br'([\s\S]*?)(\/\/[\s\S]*)')
    try:
        for line in in_file.readlines():
            m = reobj.search(line)
            if m:
                line = m.group(1).decode('cp936').encode('utf-8') + m.group(2)
            else:
                line = line.decode('cp936').encode('utf-8')
            out_file.write(line)
    finally:
        in_file.close()
        out_file.close()


def pack_ui(name):
    print('pack_ui ' + name)
    stormlib_path = path['OpenSource'] / 'StormLib' / 'Current' / 'bin' / 'Win32' / 'Release' / 'StormLib.dll'
    from_path     = path['BuildRoot']  / 'Editor' / 'UI' / 'txt' / name
    to_path       = path['Result']     / 'share' / 'mpq' / name

    fs.remove_all(from_path)
    fs.create_directories(from_path / 'ui')
    convert_file(path['UIObj'] / name, from_path / 'ui', 'TriggerData.i')
    convert_file(path['UIObj'] / name, from_path / 'ui', 'TriggerStrings.i')
    convert_file(path['UIObj'] / name, from_path / 'ui', 'WorldEditStrings.i')
    fs.copy_directory(from_path, to_path)
    
def Configuration():
    if len(sys.argv) > 1:
        return sys.argv[1]
    return 'Debug'

if __name__ == '__main__':
    util.path.ResetPath(Configuration())
    pack_ui('ydtrigger')
    pack_ui('ydwe')

