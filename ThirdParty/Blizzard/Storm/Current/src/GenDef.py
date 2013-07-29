# coding: utf-8
# Python 3 scripts

import os, sys, re

rg = re.compile(r"\s*//\s+(\d+)\s+(\w+)")

print('LIBRARY "Storm"')
print('EXPORTS')

with open("../include/BlizzardStorm.h", "r") as f:
	for line in f:
		mat = rg.match(line)
		if mat:
			print('\t{0}\t\t\t@{1}\tNONAME'.format(mat.group(2), mat.group(1)))
			