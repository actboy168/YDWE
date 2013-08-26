import sys
import os
import re
import subprocess
import time

def popen(cmd):
    return subprocess.Popen(cmd, stdin=open(os.devnull), stderr=subprocess.PIPE, stdout=subprocess.PIPE, shell=True)

def rebuild(solution, configuration, platform = 'Win32'):
    p = popen('devenv "{0}" /Rebuild "{1}|{2}"'.format(solution, configuration, platform))
    while p.poll() == None:
        sys.stdout.write(p.stdout.readline())
        #time.sleep(0.01) 
    sys.stdout.write(p.stdout.read())

def setup_env():
    keys = ['LIB', 'LIBPATH', 'PATH', 'INCLUDE']
    key_env = dict([(key, []) for key in keys])
    rekeys = {}
    for key in keys:
        rekeys[key] = re.compile('%s=(.*)' % key, re.I)

    p = popen('"{0}" & set'.format(os.path.join(os.environ['VS100COMNTOOLS'], 'vsvars32.bat')))
    for line in p.stdout.readlines():
        for k, v in rekeys.items():
            m = v.match(line)
            if m:
                for p in m.group(1).split(os.pathsep):
                    if p:
                        key_env[k].append(p.strip().strip('"').strip())
                        
    for k, l in key_env.items():
        for v in l:
            if v != '':
                try:
                    os.environ[k] = v + os.pathsep + os.environ[k]
                except:
                    os.environ[k] = v

