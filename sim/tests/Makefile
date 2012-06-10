# Makefile for all tests 
#

CC  := gcc
CXX := g++
AS  := as
LD  := ld

SUFFIXES += .d

# header files' and libraries' paths
CXXINCLUDES := ../inc
CINCLUDES   := ../inc
CXXLIBS     := ../lib
CLIBS       := ../lib

# All project files
SOURCES := $(shell ls *.cxx)
DEPS    := $(patsubst %.cxx, %.d, $(SOURCES))
EXES    := $(patsubst %.cxx, %, $(SOURCES))

# Include current directory for dynamically generated headers
CFLAGS   := -I${CINCLUDES} -I.
CXXFLAGS := -I${CXXINCLUDES} -I.
# enable this for <cstdint>
EXTFLAGS := -std=c++0x -g -O -Wall -DCONFIG_BOOKE -DCONFIG_E500 -O2

HOST_ARCH := $(shell uname -m)

EXTFLAGS += -DHOST_ARCH=$(HOST_ARCH)

# Generate the ppc_instrs as part of build process at the very beginning
EXEC_PPC_INSTR_RES := $(shell ../utils/format_instrs2.pl --ifile ../src/cpu_ppc_instr.cc --ofile cpu_ppc_instr.h)

all: $(EXES)

%.d: %.cxx
	$(CXX) $(CXXFLAGS) $(EXTFLAGS) -MM -MT '$(patsubst %.cxx, %.o, $<)' $< -MF $@

-include $(DEPS) 

%.o: %.cxx %.d
	$(CXX) $(CXXFLAGS) $(EXTFLAGS) -o $@ -c $<

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

clean:
	rm -rf *.o $(DEPS) $(EXES) *.txt *.log cpu_ppc_instr.h

distclean: clean
	echo "Done"

again: clean all
	echo "Built"


