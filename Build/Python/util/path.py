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

path = {}
path['ProjectRoot']  = BuildRoot()
path['Development']  = path['ProjectRoot'] / 'Development'
path['OpenSource']   = path['ProjectRoot'] / 'OpenSource'
path['ThirdParty']   = path['ProjectRoot'] / 'ThirdParty'
path['CoreRoot']     = path['Development'] / 'Editor' / 'Core'
path['ResultRoot']   = path['Development'] / 'Editor' / 'Build'
path['UIObj']        = path['ResultRoot'] / 'obj' / 'UI' / 'YDUI'

def ResetPath(configuration):    
    path['Result']       = path['ResultRoot'] / 'bin' / configuration
    path['ResultCore']   = path['Result'] / 'bin'
    path['ResultPlugin'] = path['Result'] / 'plugin'
