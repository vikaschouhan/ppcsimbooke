#!/usr/bin/env python
from optparse import OptionParser
import sys
sys.path.append('.')
import ppcsim

# Parse options
p = OptionParser();
p.add_option("-e", "--elf", type="string", dest="efile")
(ops, p) = p.parse_args()

if ops.efile == None:
    sys.exit("filename not specified")

# Create machine instance, load the testcase and run it
a = ppcsim.machine()
a.load_elf(ops.efile)
a.cpu0.run()

# Check results
if a.memory.read32(0x0) == 0x900d900d:
    print "%s - [PASSED]" % (ops.efile)
else:
    print "%s - [FAILED]" % (ops.efile)
