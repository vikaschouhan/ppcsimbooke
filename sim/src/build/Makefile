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
ENABLE_LOG     := 0
ENABLE_DEBUG   := 0
BUILD_FMT_UTIL := 1
ENABLE_SPE     := 0               # SPE is diabled right now

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
			 test_bm_class                    \
			 test_globals_module              \
			 test_fpu_emul_module             \
			 test_superstl                    \
			 test_basic_block_module
DUT_TEST_EXES := test_001.elf                     \
	         test_sqrt_0.elf                      \
	         test_palindrome_0.elf                \
			 test_palindrome_1.elf                \
			 test_fft_0.elf                       \
			 test_recursive_0.elf                 \
			 test_matrix_mul_0.elf                \
			 test_eval_fp_0.elf

PYWRAP_SO := ppcsim.so

EXES := $(PYWRAP_SO) $(SIM_TEST_EXES) $(DUT_TEST_EXES)

#============================ DIRS ===============================================================
# Two level up
SIM_ROOT       := ..
SIM_TESTS_ROOT := ../tests/sim
DUT_TESTS_ROOT := ../tests/dut
SIM_PY_ROOT    := $(SIM_ROOT)

#============================= HOST FLAGS ==========================================================
# Include current directory for dynamically generated headers
HOST_CFLAGS                 := -I$(SIM_ROOT)
HOST_CXXFLAGS               := -I/usr/include -I$(SIM_ROOT) -fPIC
HOST_LDFLAGS                := -L/usr/lib64 -lboost_thread

HOST_BOOST_PYTHON_CXXFLAGS  := -I`python-config --includes` -DCONFIG_BOOST_PYTHON
HOST_BOOST_PYTHON_LDFLAGS   := -lboost_python -shared

# enable this for <cstdint>
HOST_EXTFLAGS               := -std=c++0x -Wall -DCONFIG_BOOKE -DCONFIG_E500 -DCORE_TYPE=e500v2 -DCONFIG_FSL_EMB_PERFMON -O2 -DSIM_DEBUG=$(ENABLE_LOG)
HOST_EXTFLAGS               += -Wno-narrowing

HOST_ARCH                   := $(shell uname -m)
HOST_EXTFLAGS               += -DHOST_ARCH=$(HOST_ARCH)

# conditionals
ifeq ($(ENABLE_DEBUG), 1)
	HOST_EXTFLAGS    += -g
endif

#============================ TARGET FLAGS =========================================================
CROSS_CFLAGS                := -mregnames -Wa,-me500x2 -I. -I$(SIM_ROOT)
CROSS_LDFLAGS               := -L -n -N -T $(DUT_TESTS_ROOT)/e500v2_eabi_default.lcf -Wa,-me500x2
CROSS_EXTFLAGS              := -D__ASSEMBLY__ -DCONFIG_E500 -DCONFIG_BOOKE -DCONFIG_FSL_EMB_PERFMON -std=c99
CROSS_EXT_LIBS              := -lm
CROSS_OBJS_COMM             := common.o newlib_stubs.o startup.o

#
# NOTE : -me500x2 by default generates embedded booke FPU instructions.
#        Not sure, if it's a bug or something else.
#        So, we override FPU using either software emulation or SPE
ifeq ($(ENABLE_SPE), 1)
	# Enable single/double precision floating point instrs using SPE
	CROSS_EXTFLAGS   += -te500v2  -mspe
else
	# Enable floating point emulation, if SPE is disabled
	CROSS_EXTFLAGS   += -msoft-float
endif

#============================ Necessary tasks to be executed before main build ====================
# Copy run_and_check.py
EXEC_CP_RUN_PY := $(shell cp $(DUT_TESTS_ROOT)/run_and_check.py .)

#============================ Start build ==========================================================
all: $(EXES)

#======= Compile all cpp files
%.o: $(SIM_ROOT)/%.cpp $(SIM_ROOT)/%.h
	$(CXX) $(HOST_CXXFLAGS) $(HOST_EXTFLAGS) -o $@ -c $<

#======== COMPILE SIM TESTS
%.o: $(SIM_TESTS_ROOT)/%.cxx
	$(CXX) $(HOST_CXXFLAGS) $(HOST_EXTFLAGS) -o $@ -c $<

#======= COMPILE DUT TESTS
%.o: $(DUT_TESTS_ROOT)/%.c
	$(CROSS_CC) $(CROSS_CFLAGS) $(CROSS_EXTFLAGS) -c -o $@ $^

%.o: $(DUT_TESTS_ROOT)/%.S
	$(CROSS_CC) $(CROSS_CFLAGS) $(CROSS_EXTFLAGS) -c -o $@ $^

#======= BUILD DUT TESTS
test_001.elf: $(CROSS_OBJS_COMM) test_001.o
	$(CROSS_CC) $(CROSS_LDFLAGS) $(CROSS_EXTFLAGS) -Wl,-Map=$(@).map -o $(@) $^ $(CROSS_EXT_LIBS)

test_sqrt_0.elf: $(CROSS_OBJS_COMM) test_sqrt_0.o
	$(CROSS_CC) $(CROSS_LDFLAGS) $(CROSS_EXTFLAGS) -Wl,-Map=$(@).map -o $(@) $^ $(CROSS_EXT_LIBS)

test_palindrome_0.elf: $(CROSS_OBJS_COMM) test_palindrome_0.o
	$(CROSS_CC) $(CROSS_LDFLAGS) $(CROSS_EXTFLAGS) -Wl,-Map=$(@).map -o $(@) $^ $(CROSS_EXT_LIBS)

test_palindrome_1.elf: $(CROSS_OBJS_COMM) test_palindrome_1.o
	$(CROSS_CC) $(CROSS_LDFLAGS) $(CROSS_EXTFLAGS) -Wl,-Map=$(@).map -o $(@) $^ $(CROSS_EXT_LIBS)

test_fft_0.elf: $(CROSS_OBJS_COMM) test_fft_0.o
	$(CROSS_CC) $(CROSS_LDFLAGS) $(CROSS_EXTFLAGS) -Wl,-Map=$(@).map -o $(@) $^ $(CROSS_EXT_LIBS)

test_recursive_0.elf: $(CROSS_OBJS_COMM) test_recursive_0.o
	$(CROSS_CC) $(CROSS_LDFLAGS) $(CROSS_EXTFLAGS) -Wl,-Map=$(@).map -o $(@) $^ $(CROSS_EXT_LIBS)

test_matrix_mul_0.elf: $(CROSS_OBJS_COMM) test_matrix_mul_0.o
	$(CROSS_CC) $(CROSS_LDFLAGS) $(CROSS_EXTFLAGS) -Wl,-Map=$(@).map -o $(@) $^ $(CROSS_EXT_LIBS)

test_eval_fp_0.elf: $(CROSS_OBJS_COMM) test_eval_fp_0.o
	$(CROSS_CC) $(CROSS_LDFLAGS) $(CROSS_EXTFLAGS) -Wl,-Map=$(@).map -o $(@) $^ $(CROSS_EXT_LIBS)

dut_tests: $(DUT_TEST_EXES)

#======= BUILD SIMULATOR MODULE
ppcsim.o: $(SIM_ROOT)/ppcsimbooke.cpp
	$(CXX) $(HOST_CXXFLAGS) $(HOST_BOOST_PYTHON_CXXFLAGS) $(HOST_LDFLAGS) $(HOST_BOOST_PYTHON_LDFLAGS) $(HOST_EXTFLAGS) -o $@  -c $<

ppcsim.so: ppcsim.o machine.o cpu_ppc.o ppc_dis.o tlb_booke.o memory.o cpu_ppc_coverage.o globals.o bm.o
	$(CXX) $(HOST_CXXFLAGS) $(HOST_BOOST_PYTHON_CXXFLAGS) $(HOST_LDFLAGS) $(HOST_BOOST_PYTHON_LDFLAGS) $(HOST_EXTFLAGS) -Wl,-soname,"$@"  -o $@ $^

ppcsimbooke: ppcsim.so

#====== BUILD SIM TESTS
test_ppcdis_interface: test_ppcdis_interface.o ppc_dis.o globals.o
	$(CXX) -o $@ $^

test_machine_interface: test_machine_interface.o machine.o cpu_ppc.o ppc_dis.o tlb_booke.o memory.o cpu_ppc_coverage.o globals.o bm.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_cpu_ppc_interface: test_cpu_ppc_interface.o cpu_ppc.o ppc_dis.o tlb_booke.o memory.o cpu_ppc_coverage.o globals.o bm.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_cpu_ppc_coverage_interface: test_cpu_ppc_coverage_interface.o cpu_ppc_coverage.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_tlb_booke_interface: test_tlb_booke_interface.o tlb_booke.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_memory_interface: test_memory_interface.o  memory.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_exception_class: test_exception_class.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_log_class: test_log_class.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_lru_class: test_lru_class.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_bm_class: test_bm_class.o bm.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_globals_module: test_globals_module.o globals.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_fpu_emul_module: test_fpu_emul_module.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_superstl: test_superstl.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

test_basic_block_module: test_basic_block_module.o basic_block.o globals.o cpu_ppc.o ppc_dis.o tlb_booke.o memory.o cpu_ppc_coverage.o bm.o
	$(CXX) $(HOST_LDFLAGS) -o $@ $^

sim_tests: $(SIM_TEST_EXES)

clean:
	rm -rf *.o $(EXES) *.txt *.log cpu_ppc_instr.inc format_instrs *.tcc *.so *.elf *.map *.py

distclean: clean
	echo "Done"

again: clean all
	echo "Built"

run_dut_tests: $(DUT_TEST_EXES) ppcsim.so
	$(foreach var, $(DUT_TEST_EXES), ./run_and_check.py --file $(var);)

run_sim_tests: $(SIM_TEST_EXES)
	$(foreach var, $(SIM_TEST_EXES), ./$(var);)

run_tests: run_dut_tests run_sim_tests

show_dut_tests:
	@echo $(DUT_TEST_EXES)

show_sim_tests:
	@echo $(SIM_TEST_EXES)

