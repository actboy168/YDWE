# -*- coding: utf8 -*-

import sys
import util.path
import util.zip_package as zip

path = util.path.path

def pack_skin():
    print('pack_skin')
    zip.zip_folder(str(path['CoreRoot'] / 'DuiLib' / 'Resources'), str(path['ResultCore'] / 'skin.zip'), zip.FileNameRegexNegtiveFilter('thumbs\.db'))

def Configuration():
    if len(sys.argv) > 1:
        return sys.argv[1]
    return 'Debug'

if __name__ == '__main__':
    util.path.ResetPath(Configuration())
    pack_skin()
