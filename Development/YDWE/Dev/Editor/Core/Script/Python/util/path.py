# -*- coding: utf8 -*-

import os
import util.filesystem as fs

#和[MSBuild]::GetDirectoryNameOfFileAbove效果相同
def GetDirectoryNameOfFileAbove(p, f):
    p = fs.path(p)
    while True:
        if fs.exists(p / f):
            return p
        p.remove_filename()
        if p.filename() == '':
            break
    return None

def BuildRoot():
    ret = GetDirectoryNameOfFileAbove(os.getcwd(), 'build.root')
    if ret is None:
        raise Exception, 'not found build.root.'
    return ret




