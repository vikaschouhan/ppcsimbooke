# Makefile for all tests 
#

CC  := gcc 
CXX := g++
AS  := as
LD  := ld

# User defined flags
USRFLAGS :=

SUFFIXES += .d

# Two level up
SRCROOT  := ../..

# header files' and libraries' paths
CXXINCLUDES := $(SRCROOT)/inc
CINCLUDES   := $(SRCROOT)/inc
CXXLIBS     := $(SRCROOT)/lib
CLIBS       := $(SRCROOT)/lib

# All project files
SOURCES := $(shell ls *.cxx)
DEPS    := $(patsubst %.cxx, %.d, $(SOURCES))
EXES    := $(patsubst %.cxx, %, $(SOURCES))

# Include current directory for dynamically generated headers
CFLAGS   := -I${CINCLUDES} -I.
CXXFLAGS := -I${CXXINCLUDES} -I.
# enable this for <cstdint>
EXTFLAGS := -std=c++0x -g -O -Wall -DCONFIG_BOOKE -DCONFIG_E500 -DCORE_TYPE=e500v2 -DCONFIG_FSL_EMB_PERFMON -O2 $(USRFLAGS)

HOST_ARCH := $(shell uname -m)

EXTFLAGS += -DHOST_ARCH=$(HOST_ARCH)

# Generate the ppc_instrs as part of build process at the very beginning
EXEC_PPC_INSTR_RES := $(shell $(SRCROOT)/utils/format_instrs2.pl --ifile $(SRCROOT)/src/cpu_ppc_instr.cc --ofile cpu_ppc_instr.h)

all: $(EXES)

%.d: %.cxx
	$(CXX) $(CXXFLAGS) $(EXTFLAGS) -MM -MT '$(patsubst %.cxx, %.o, $<)' $< -MF $@

-include $(DEPS) 

%.o: %.cxx %.d
	$(CXX) $(CXXFLAGS) $(EXTFLAGS) -o $@ -c $<

test_ppcdis_interface: test_ppcdis_interface.o
	$(CXX) -o $@ $^

test_machine_interface: test_machine_interface.o
	$(CXX) -o $@ $^

test_cpu_ppc_interface: test_cpu_ppc_interface.o
	$(CXX) -o $@ $^

test_tlb_booke_interface: test_tlb_booke_interface.o
	$(CXX) -o $@ $^

test_memory_interface: test_memory_interface.o
	$(CXX) -o $@ $^

test_exception_class: test_exception_class.o
	$(CXX) -o $@ $^

test_log_class: test_log_class.o
	$(CXX) -o $@ $^

test_lru_class: test_lru_class.o
	$(CXX) -o $@ $^

clean:
	rm -rf *.o $(DEPS) $(EXES) *.txt *.log cpu_ppc_instr.h

distclean: clean
	echo "Done"

again: clean all
	echo "Built"


