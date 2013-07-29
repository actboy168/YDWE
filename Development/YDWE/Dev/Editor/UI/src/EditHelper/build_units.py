
import os, sys, re
import shutil
import StormLib

StormLibPath = '/../../../../../OpenSource/StormLib/Current/bin/Win32/Release/StormLib.dll'

def reset_dir(path):
  try:
    os.remove(path)
  except:
    pass
  try:
    os.makedirs(path)
  except:
    pass
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
  if len(sys.argv) < 2:
    return
  path = sys.argv[1]
  txt_path = path + '/txt/units/'
  bin_path = path + '/bin/'
  reset_dir(txt_path + 'ui/')
  reset_dir(txt_path + 'units/')
  reset_dir(txt_path + 'doodads/')
  try:
    os.makedirs(bin_path)
  except:
    pass
  copy_dir(path + '/src/Units/ui',      txt_path + '/ui')
  copy_dir(path + '/src/Units/units',   txt_path + '/units')
  copy_dir(path + '/src/Units/doodads', txt_path + '/doodads')
  create_mpq(path + StormLibPath, txt_path, bin_path + 'units.mpq')
###############################################################
if __name__ == "__main__":
  main()
