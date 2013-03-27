#!/usr/bin/env python
from optparse import OptionParser
import sys
import time
sys.path.append('.')
import ppcsim

class bcolors:
    OKGREEN   = '\033[92m'
    FAIL      = '\033[93m'
    ENDC      = '\033[0m'

# All variables
trace_start_addr = 0x100000;
trace_size       = 0x1000;      # Extracting only 4K space.

# Parse options
p = OptionParser();
p.add_option("-f", "--file", type="string", dest="efile")
(ops, p) = p.parse_args()

if ops.efile == None:
    sys.exit("filename not specified")

# Create a trace file name for tracing print statments
tr_file = ops.efile + ".log"

# Create machine instance, load the testcase and run it
a = ppcsim.machine()
a.load_elf(ops.efile)
a.run()

# TODO : We should implement some sort of time out feature ( in case testcase hangs )
while a.memory.read32(0x0) != 0x80000001:
    pass
while ((a.memory.read32(0x4) != 0x900d900d) and (a.memory.read32(0x4) != 0xbaadbaad)):
    pass

# Stop the machine and sleep for w while
a.stop()
time.sleep(1)
# Get trace file
a.memory.read_to_ascii_file(trace_start_addr, tr_file, trace_size)

# Check results
if a.memory.read32(0x4) == 0x900d900d:
    print bcolors.OKGREEN + "%s - [PASSED]" % (ops.efile) + bcolors.ENDC
else:
    print bcolors.FAIL + "%s - [FAILED]" % (ops.efile) + bcolors.ENDC

# Generate coverage logs
a.gen_cov_logs()
