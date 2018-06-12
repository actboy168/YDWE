LIBNAME = lpeglabel
LUADIR = ../lua/

COPT = -O2
# COPT = -DLPEG_DEBUG -g

CWARNS = -Wall -Wextra -pedantic \
	-Waggregate-return \
	-Wcast-align \
	-Wcast-qual \
	-Wdisabled-optimization \
	-Wpointer-arith \
	-Wshadow \
	-Wsign-compare \
	-Wundef \
	-Wwrite-strings \
	-Wbad-function-cast \
	-Wdeclaration-after-statement \
	-Wmissing-prototypes \
	-Wnested-externs \
	-Wstrict-prototypes \
# -Wunreachable-code \


CFLAGS = $(CWARNS) $(COPT) -std=c99 -I$(LUADIR) -fPIC
CC = gcc

FILES = lpvm.o lpcap.o lptree.o lpcode.o lpprint.o
# For Linux
linux:
	make lpeglabel.so "DLLFLAGS = -shared -fPIC"

# For Mac OS
macosx:
	make lpeglabel.so "DLLFLAGS = -bundle -undefined dynamic_lookup"

# For Windows
windows:
	make lpeglabel.dll "DLLFLAGS = -shared -fPIC"

lpeglabel.so: $(FILES)
	env $(CC) $(DLLFLAGS) $(FILES) -o lpeglabel.so
lpeglabel.dll: $(FILES)
	$(CC) $(DLLFLAGS) $(FILES) -o lpeglabel.dll $(LUADIR)/bin/lua53.dll

$(FILES): makefile

test: test.lua testlabel.lua testrelabelparser.lua relabel.lua lpeglabel.so
	lua test.lua
	lua testlabel.lua
	lua testrelabelparser.lua

clean:
	rm -f $(FILES) lpeglabel.so


lpcap.o: lpcap.c lpcap.h lptypes.h
lpcode.o: lpcode.c lptypes.h lpcode.h lptree.h lpvm.h lpcap.h
lpprint.o: lpprint.c lptypes.h lpprint.h lptree.h lpvm.h lpcap.h
lptree.o: lptree.c lptypes.h lpcap.h lpcode.h lptree.h lpvm.h lpprint.h
lpvm.o: lpvm.c lpcap.h lptypes.h lpvm.h lpprint.h lptree.h

