# coding: utf-8

import argparse, sys, os, zipfile, re

def zip_folder(folder_name, file_name, file_name_filter = None):
	cur_dir = os.getcwd()
	os.chdir(folder_name)
	try:
		print("Packaging zip file: " + file_name)
		print("Source directory: " + folder_name)

		with zipfile.ZipFile(file_name, "w", zipfile.ZIP_DEFLATED) as zip:
			for root, dirs, files in os.walk(os.curdir):
				count = 0
				for file in files:
					archive_file = os.path.join(root[2:], file)
					if file_name_filter is not None and not file_name_filter(file):
						print("  Filtering: " + archive_file)
					else:
						print("  Adding: " + archive_file)
						zip.write(archive_file)
						count += 1
				
				if count == 0:
					archive_file = root[2:] + "/"
					print("  Adding empty directory: " + archive_file)
					zif = zipfile.ZipInfo(archive_file)
					zip.writestr(zif, "")
					
		print("Package completed.")
		
	finally:
		os.chdir(cur_dir)

# Regex matches -> False
# Regex does not match -> True
class FileNameRegexNegtiveFilter(object):
	def __init__(self, filter_regex):
		self.filter_regex = re.compile(filter_regex, re.IGNORECASE)

	def __call__(self, file_name):
		return not self.filter_regex.match(file_name)

if __name__ == "__main__":
	parser = argparse.ArgumentParser(
		description = "Script utility for zipping a directory."
	)
	parser.add_argument("-s", action = "store", dest = "src", required = True)
	parser.add_argument("-d", action = "store", dest = "dest", required = True)
	parser.add_argument("-f", action = "store", dest = "filter_regex")
	
	arg_info = vars(parser.parse_args())
	file_name_filter = None
	if arg_info["filter_regex"]:
		file_name_filter = FileNameRegexNegtiveFilter(arg_info["filter_regex"])
	zip_folder(arg_info["src"], arg_info["dest"], file_name_filter)
	