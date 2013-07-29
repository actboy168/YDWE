# -*- coding: utf-8 -*-

import sys
import os
import re

###############################################################
def write_header(f):
  f.write('#if 0\n')
  f.write('//--------------------------------------------//\n')
  f.write('//         本文件为自动生成，请勿编辑         //\n')
  f.write('//             thanks to 最萌小汐             //\n')
  f.write('//--------------------------------------------//\n')
  f.write('#endif\n')
  
###############################################################
def create_file(name, buf, outputpath):
  
  reobj = re.compile(r' YDWE([a-zA-Z0-9_]*)Null')
  
  func_list = []
  for match in reobj.finditer(buf):
    if name != match.group(1):
      func_list[0:0] = [match.group(1)]
      #print name + ' - ' + match.group(1)
  func_list = list(set(func_list))
  
  try:
    outfile  = file(outputpath + '/detail/' + name + ".j", "w")
    try:      
      write_header(outfile)
      outfile.write('#ifdef USE_BJ_ANTI_LEAK\n')
      outfile.write('#ifndef YDWE' + name + 'NullIncluded\n')
      outfile.write('#define YDWE' + name + 'NullIncluded\n')
      outfile.write('\n')

      for func in func_list:
        outfile.write('#include "AntiBJLeak/detail/'+ func + '.j"\n')
      outfile.write('\n')

      if 0 == len(func_list):
        outfile.write('library YDWE' + name + 'Null\n')
      else:
        outfile.write('library YDWE' + name + 'Null requires')
        first_func = 1
        for func in func_list:
          if first_func == 1:
            outfile.write(' YDWE' + func + 'Null')
            first_func = 0
          else:
            outfile.write(', YDWE' + func + 'Null')
        outfile.write('\n')
        
      outfile.write('\n')

      reobj = re.compile(r'yd_NullTempCam')
      if reobj.search(buf):
        outfile.write('globals\n')
        outfile.write('#ifndef YDWE_NNULLTEMPCAM_DEFVAR\n')
        outfile.write('#define YDWE_NNULLTEMPCAM_DEFVAR\n')
        outfile.write('    camerasetup yd_NullTempCam\n')
        outfile.write('#endif\n')
        outfile.write('endglobals\n\n')
        
      reobj = re.compile(r'yd_NullTempRegion')
      if reobj.search(buf):
        outfile.write('globals\n')
        outfile.write('#ifndef YDWE_NNULLTEMPREGION_DEFVAR\n')
        outfile.write('#define YDWE_NNULLTEMPREGION_DEFVAR\n')
        outfile.write('    region yd_NullTempRegion\n')
        outfile.write('#endif\n')
        outfile.write('endglobals\n\n')
        
      reobj = re.compile(r'yd_NullTempUnit')
      if reobj.search(buf):
        outfile.write('globals\n')
        outfile.write('#ifndef YDWE_NNULLTEMPUNIT_DEFVAR\n')
        outfile.write('#define YDWE_NNULLTEMPUNIT_DEFVAR\n')
        outfile.write('    unit yd_NullTempUnit\n')
        outfile.write('#endif\n')
        outfile.write('endglobals\n\n')

      reobj = re.compile(r'yd_NullTempItem')
      if reobj.search(buf):
        outfile.write('globals\n')
        outfile.write('#ifndef YDWE_NNULLTEMPITEM_DEFVAR\n')
        outfile.write('#define YDWE_NNULLTEMPITEM_DEFVAR\n')
        outfile.write('    item yd_NullTempItem\n')
        outfile.write('#endif\n')
        outfile.write('endglobals\n\n')
        
      reobj = re.compile(r'yd_NullTempGroup')
      if reobj.search(buf):
        outfile.write('globals\n')
        outfile.write('#ifndef YDWE_NNULLTEMPGROUP_DEFVAR\n')
        outfile.write('#define YDWE_NNULLTEMPGROUP_DEFVAR\n')
        outfile.write('    group yd_NullTempGroup\n')
        outfile.write('#endif\n')
        outfile.write('endglobals\n\n')
        
      reobj = re.compile(r'yd_NullTempForce')
      if reobj.search(buf):
        outfile.write('globals\n')
        outfile.write('#ifndef YDWE_NNULLTEMPFORCE_DEFVAR\n')
        outfile.write('#define YDWE_NNULLTEMPFORCE_DEFVAR\n')
        outfile.write('    force yd_NullTempForce\n')
        outfile.write('#endif\n')
        outfile.write('endglobals\n\n')
        
      outfile.write(buf)
      outfile.write('\n')
      outfile.write('\n')
      outfile.write('endlibrary\n')
      outfile.write('\n')
      outfile.write('#endif\n')
      outfile.write('#endif\n')
    finally:
      outfile.close()
  except IOError:
    pass
  try:
    outfile  = file(outputpath + '/detail/' + name + '.cfg', 'w')
    try:
      outfile.write(name + '\n')
    finally:
      outfile.close()
  except IOError:
    pass
###############################################################
def write_def_beg(def_file):
  write_header(def_file)
  def_file.write('#ifdef USE_BJ_ANTI_LEAK\n')
  def_file.write('\n')
  def_file.write('#ifndef INCLUDE_YDWE_ANTI_BJ_LEAK_H\n')
  def_file.write('#define INCLUDE_YDWE_ANTI_BJ_LEAK_H\n')
  def_file.write('\n')
  def_file.write('#ifdef SCRIPT_INJECTION\n')
  def_file.write('\n')
###############################################################
def write_def_end(def_file):
  def_file.write('\n')
  def_file.write('#endif\n')
  def_file.write('#endif\n')
  def_file.write('#endif\n')
###############################################################
def get_param_count(buf):
  buf = buf.strip()
  if buf == 'nothing':
    return 0
  return len(buf.split(','))
###############################################################
def write_def(name, n, def_file):
  l = ''
  if n != 0:
    l = 'a'
    for i in range(1,n):
      l = l + ',' + chr(ord('a')+i)
  def_file.write('#define %s(%s)' % (name, l))
  for i in range(1,41-len(name)-len(l)):
    def_file.write(' ')
  def_file.write('YDWE%sNull(%s)\n' % (name, l))
###############################################################
def parse_file(filename, outputpath):
  try:
    os.makedirs(outputpath + '/detail/')
  except:
    pass
  try:
    infile  = file(filename, 'r')
    def_file = file(outputpath+'/MainMacro.h', 'w')
    try:
      write_def_beg(def_file)
      reobj = re.compile(r'function YDWE([a-zA-Z0-9_]*)Null takes([\s\S]*?)returns[\s\S]*?endfunction')
      for match in reobj.finditer(infile.read()):
        create_file(match.group(1), match.group(), outputpath)
        write_def(match.group(1), get_param_count(match.group(2)), def_file)
      write_def_end(def_file)
    finally:
      infile.close()
      def_file.close()
  except IOError:
    pass
###############################################################
if __name__ == "__main__":
  if len(sys.argv) >= 3:
    outputpath = sys.argv[2]
  else:
    outputpath = './'
  if len(sys.argv) >= 2: 
    inputpath = sys.argv[1]
  else:
    inputpath = './bj_script.j'
  parse_file(inputpath, outputpath)
