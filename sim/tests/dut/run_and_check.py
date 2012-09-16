#!/usr/bin/env python
from optparse import OptionParser
import sys
import time
sys.path.append('.')
import ppcsim

# Parse options
p = OptionParser();
p.add_option("-f", "--file", type="string", dest="efile")
(ops, p) = p.parse_args()

if ops.efile == None:
    sys.exit("filename not specified")

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

# Check results
if a.memory.read32(0x4) == 0x900d900d:
    print "%s - [PASSED]" % (ops.efile)
else:
    print "%s - [FAILED]" % (ops.efile)

# Generate coverage logs
a.gen_cov_logs()
