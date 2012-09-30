# Makefile for all tests 
#

CROSS_PREFIX   := powerpc-eabi-

CC             := gcc 
CXX            := g++
AS             := as
LD             := ld
CROSS_CC       := $(CROSS_PREFIX)gcc
CROSS_AS       := $(CROSS_PREFIX)as
CROSS_LD       := $(CROSS_PREFIX)ld
CROSS_OBJDUMP  := $(CROSS_PREFIX)objdump

#============================= user defined flags ===============================================
ENABLE_LOG     := 1
ENABLE_DEBUG   := 0
BUILD_FMT_UTIL := 1

#============================= Define All EXE targets here =======================================
# Exes for tests
SIM_TEST_EXES := test_ppcdis_interface        \
	         test_machine_interface           \
             test_cpu_ppc_interface           \
			 test_cpu_ppc_coverage_interface  \
             test_tlb_booke_interface         \
             test_memory_interface            \
             test_exception_class             \
             test_log_class                   \
			 test_lru_class                   \
			 test_bm_class
DUT_TEST_EXES := test_001                     \
	         test_palindrome_0                \
			 test_palindrome_1
PYWRAP_SO := ppcsim

EXES := $(PYWRAP_SO) $(SIM_TEST_EXES) $(DUT_TEST_EXES)

#============================ DIRS ===============================================================
# Two level up
SIM_ROOT       := ..
SIM_TESTS_ROOT := ../tests/sim
DUT_TESTS_ROOT := ../tests/dut
SIM_PY_ROOT    := ../wrapper/python

# header files' and libraries' paths
CXXINCLUDES := $(SIM_ROOT)/inc
CINCLUDES   := $(SIM_ROOT)/inc
#CXXLIBS     := $(SIM_ROOT)/lib
#CLIBS       := $(SIM_ROOT)/lib

#============================= HOST FLAGS ==========================================================
# Include current directory for dynamically generated headers
HOST_CFLAGS                 := -I${CINCLUDES} -I.
HOST_CXXFLAGS               := -I${CXXINCLUDES} -I. -I/usr/include/boost
HOST_LDFLAGS                := -L/usr/lib64 -lboost_thread

HOST_BOOST_PYTHON_CXXFLAGS  := -I`python-config --includes` -DCONFIG_BOOST_PYTHON
HOST_BOOST_PYTHON_LDFLAGS   := -lboost_python -shared

# enable this for <cstdint>
HOST_EXTFLAGS               := -std=c++0x -O -Wall -DCONFIG_BOOKE -DCONFIG_E500 -DCORE_TYPE=e500v2 -DCONFIG_FSL_EMB_PERFMON -O2 -DSIM_DEBUG=$(ENABLE_LOG)

HOST_ARCH                   := $(shell uname -m)
HOST_EXTFLAGS               += -DHOST_ARCH=$(HOST_ARCH)

# conditionals
ifeq ($(ENABLE_DEBUG), 1)
	HOST_EXTFLAGS    += -g
endif

#============================ TARGET FLAGS =========================================================
CROSS_CFLAGS                := -mregnames -Wa,-me500x2 -Wa,-mspe -I. -I$(SIM_ROOT)/inc 
CROSS_LDFLAGS               := -L -n -N -T $(DUT_TESTS_ROOT)/e500v2_eabi_default.lcf -Wa,-me500x2 -Wa,-mspe
CROSS_EXTFLAGS              := -D__ASSEMBLY__ -DCONFIG_E500 -DCONFIG_BOOKE -DCONFIG_FSL_EMB_PERFMON -std=c99
CROSS_OBJS_COMM             := common.o newlib_stubs.o startup.o

#============================ BUILD formatting UTILITY =============================================
# Buid formatting utility ( only once )
ifeq ($(BUILD_FMT_UTIL), 1)
    EXEC_PPC_INSTR_RES := $(shell g++ $(HOST_CXXFLAGS) $(HOST_EXTFLAGS) -lboost_program_options -o format_instrs $(SIM_ROOT)/utils/format_instrs.cpp ; \
                       ./format_instrs --ifile $(SIM_ROOT)/src/cpu_ppc_instr.cc --ofile cpu_ppc_instr.inc)
endif
# Copy run_and_check.py
    EXEC_CP_RUN_PY := $(shell cp $(DUT_TESTS_ROOT)/run_and_check.py .)

#============================ Start build ==========================================================
all: $(EXES)

#======== COMPILE SIM TESTS
%.o: $(SIM_TESTS_ROOT)/%.cxx
	$(CXX) $(HOST_CXXFLAGS) $(HOST_EXTFLAGS) -o $@ -c $<

#======= COMPILE DUT TESTS
%.o: $(DUT_TESTS_ROOT)/%.c
	$(CROSS_CC) $(CROSS_CFLAGS) $(CROSS_EXTFLAGS) -c -o $@ $^

%.o: $(DUT_TESTS_ROOT)/%.S
	$(CROSS_CC) $(CROSS_CFLAGS) $(CROSS_EXTFLAGS) -c -o $@ $^

#======= BUILD DUT TESTS
test_001: $(CROSS_OBJS_COMM) test_001.o
	$(CROSS_CC) $(CROSS_LDFLAGS) $(CROSS_EXTFLAGS) -Wl,-Map=$(@).map -o $(@).elf $^

test_palindrome_0: $(CROSS_OBJS_COMM) test_palindrome_0.o
	$(CROSS_CC) $(CROSS_LDFLAGS) $(CROSS_EXTFLAGS) -Wl,-Map=$(@).map -o $(@).elf $^

test_palindrome_1: $(CROSS_OBJS_COMM) test_palindrome_1.o
	$(CROSS_CC) $(CROSS_LDFLAGS) $(CROSS_EXTFLAGS) -Wl,-Map=$(@).map -o $(@).elf $^

dut_tests: $(DUT_TEST_EXES)

#======= BUILD SIMULATOR MODULE
ppcsim: $(SIM_PY_ROOT)/ppcsimbooke.cpp
	$(CXX) $(HOST_CXXFLAGS) $(HOST_BOOST_PYTHON_CXXFLAGS) $(HOST_LDFLAGS) $(HOST_BOOST_PYTHON_LDFLAGS) $(HOST_EXTFLAGS) -Wl,-soname,"$@.so"  -fpic -o $@.so $^

#====== BUILD SIM TESTS
test_ppcdis_interface: test_ppcdis_interface.o
	$(CXX) -o $@ $^

test_machine_interface: test_machine_interface.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_cpu_ppc_interface: test_cpu_ppc_interface.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_cpu_ppc_coverage_interface: test_cpu_ppc_coverage_interface.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_tlb_booke_interface: test_tlb_booke_interface.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_memory_interface: test_memory_interface.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_exception_class: test_exception_class.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_log_class: test_log_class.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_lru_class: test_lru_class.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_bm_class: test_bm_class.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

sim_tests: $(SIM_TEST_EXES)

clean:
	rm -rf *.o $(EXES) *.txt *.log cpu_ppc_instr.inc format_instrs *.tcc *.so *.elf *.map *.py

distclean: clean
	echo "Done"

again: clean all
	echo "Built"


