SHELL = /bin/bash

MAIN_FILE = main
#leave out .c extension

CUDD_PATH = /home/akshay/Colg_files/MCSV/work/cudd-3.0.0
CC = gcc -std=gnu11

cudd_inc = -I $(CUDD_PATH)/cudd -I $(CUDD_PATH)/util -I $(CUDD_PATH) -I $(CUDD_PATH)/cplusplus
cudd_libs = -Bdynamic -L $(CUDD_PATH)/cudd/.libs -lcudd -lm 
#use dynamic linking for GC. Set Rpath for CUDD so that lib can be loaded at runtime.
cudd_rpath = -Wl,-R$(CUDD_PATH)/cudd/.libs/
cudd_all = $(cudd_inc) $(cudd_libs) $(cudd_rpath)

gc_inc = -I /usr/include/gc/
gc_lib = -Bdynamic -lgc
gc_all = $(gc_inc) $(gc_lib)

#----------------MAKE SPEC------------------
.DEFAULT: default

.PHONY: default debug clean

#-------------- TARGET RULES ---------------

sharedlib_flags = -Bstatic -L . -lddStack -L . -ltokenisation

default: $(MAIN_FILE).c libddStack.a libtokenisation.a
	$(CC) $(DBGCFLAGS) $(MAIN_FILE).c $(gc_all) $(cudd_all) $(sharedlib_flags) -o $(MAIN_FILE) 

debug: DBGCFLAGS += -g -O0 -fstack-protector-all -Wall -Wno-unused-variable -Wno-unused-value -Wno-unused-but-set-variable 
debug: $(MAIN_FILE).c default

clean: 
	rm *.o *.a
#-------------------Libs--------------------

libddStack.a: ddStack.c 
	$(CC) -c $(sharedlib_flags) $(DBGCFLAGS) ddStack.c $(cudd_all) -o libddStack.o
	ar rcs libddStack.a libddStack.o

libtokenisation.a: tokenisation.c tokenisation.h
	$(CC) -c $(sharedlib_flags) $(DBGCFLAGS) tokenisation.c -o libtokenisation.o
	ar rcs libtokenisation.a libtokenisation.o

#-------------------------------------------