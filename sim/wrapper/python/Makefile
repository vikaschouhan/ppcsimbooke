# Makefile for all tests 
#

CC     := cc
CXX    := c++
AS     := as
LD     := ld
FMT_PL := format_instrs2.pl

SUFFIXES += .d

# header files' and libraries' paths
CXXFLAGS        := -I../../inc -I`python-config --includes` -I/usr/include/boost -I.
CXXFLAGS        += -L/usr/lib64 -lboost_python

# enable this for <cstdint>
CXXFLAGS   += -std=c++0x -g -O -Wall -DCONFIG_BOOKE -DCONFIG_E500 -DCORE_TYPE=e500v2 -DCONFIG_FSL_EMB_PERFMON -O2 -fpic -shared

HOST_ARCH := $(shell uname -m)
CXXFLAGS += -DHOST_ARCH=$(HOST_ARCH)

# Generate the ppc_instrs as part of build process at the very beginning
EXEC_PPC_INSTR_RES := $(shell ../../utils/$(FMT_PL) --ifile ../../src/cpu_ppc_instr.cc --ofile cpu_ppc_instr.h)

# Generate the ppc_instrs as part of build process at the very beginning
all: ppcsim

ppcsim: ppcsimbooke.cpp
	$(CXX) $(CXXFLAGS) -Wl,-soname,"$@.so"  -fpic -o $@.so $^

clean:
	rm -rf *.so *.h *.log

distclean: clean
	echo "Done"

again: clean all
	echo "Built"


