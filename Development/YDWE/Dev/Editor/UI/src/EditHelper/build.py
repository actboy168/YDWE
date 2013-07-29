import argparse
import os, sys, re
import shutil
import StormLib

StormLibPath = '/../../../../../OpenSource/StormLib/Current/bin/Win32/Release/StormLib.dll'

def fine_comment(buf):
    pass
###############################################################
def convert_file(srcpath, destpath, filename):
  #try:
    spext = os.path.splitext(filename)
    ifile = open(srcpath + filename, "br")
    ofile = open(destpath + spext[0]+'.txt', "bw")
    reobj = re.compile(br'([\s\S]*?)(\/\/[\s\S]*)')
    try:
      for line in ifile.readlines():
        match = reobj.search(line)
        if match:
            line = match.group(1).decode('cp936').encode('utf-8') + match.group(2)
        else:
          line = line.decode('cp936').encode('utf-8')
        ofile.write(line)
    finally:
      ifile.close()
      ofile.close()
  #except IOError:
  #  pass
###############################################################
def copy_dir(srcpath, dstpath):
  if os.path.isdir(srcpath):
    for name in os.listdir(srcpath):
      filePath = os.path.join(srcpath, name)
      if os.path.isfile(filePath):
        try:
          shutil.copy(filePath, os.path.join(dstpath, name))
        except:
          pass
      elif os.path.isdir(filePath):
        copy_dir(filePath, os.path.join(dstpath, name))
###############################################################
def mpq_add_dir(mpq, srcpath, srcname):
  for name in os.listdir(os.path.join(srcpath, srcname)):
    file_path = os.path.join(os.path.join(srcpath, srcname), name)
    if os.path.isfile(file_path):
      try:
        mpq.AddFileEx(file_path.encode('ascii'), os.path.join(srcname, name).encode('ascii'))
      except:
        pass
    elif os.path.isdir(file_path):
      mpq_add_dir(mpq, srcpath, os.path.join(srcname, name))
###############################################################
def create_mpq(stormlib_path, srcpath, dstpath):
  stormlib = StormLib.Open(stormlib_path)
  #try:
  #  os.remove(dstpath) 
  #except:
  #  pass
  mpq = stormlib.Open(dstpath.encode('ascii'))
  if mpq is None:
    raise Exception('Can\'t open mpq ' + dstpath)
  mpq_add_dir(mpq, srcpath, '')
  mpq.Compact()
  del mpq
###############################################################
def main():
  parser = argparse.ArgumentParser()
  parser.add_argument("-s", action = "store", dest = "src", required = True)
  parser.add_argument("-d", action = "store", dest = "dst", required = True)
  parser.add_argument("-n", action = "store", dest = "name", required = True)
  arg_info = vars(parser.parse_args())
  
  txt_path = arg_info["dst"] + '/txt/' + arg_info["name"] + '/'
  bin_path = arg_info["dst"] + '/bin/'
  try:
    os.makedirs(txt_path + 'ui/')
  except:
    pass
  try:
    os.makedirs(bin_path)
  except:
    pass
  convert_file(arg_info["src"], txt_path + 'ui/', 'TriggerData.i')
  convert_file(arg_info["src"], txt_path + 'ui/', 'TriggerStrings.i')
  convert_file(arg_info["src"], txt_path + 'ui/', 'WorldEditStrings.i')
  create_mpq(arg_info["dst"] + StormLibPath, txt_path, bin_path + arg_info["name"]+ '.mpq')
###############################################################
if __name__ == "__main__":
  main()
