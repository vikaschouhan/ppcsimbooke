#ifndef _CONFIG_H
#define _CONFIG_H

// Include all common std. header files
#include <iostream>
#include <fstream>
#include <cmath>
#include <exception>
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <tuple>
#include <bitset>
#include <string>
#include <cstdint>
#include <cstdarg>
#include <iomanip>
#include <cassert>
#include <ctime>
#include <cstring>
#include <sstream>
#include <utility>

// Define register masks/fields
#include "third_party/ppc/reg.h"
#include "regfields.h"

// Boost headers
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

// Include all common misc local header files
#include "exception.hpp"
#include "ppc_exception.h"
#include "log.hpp"
#include "utils.h"

#include "globals.h"

// check for little endianness & x86
#if !(defined __ORDER_LITTLE_ENDIAN__ && (defined __x86_64__ || defined __x86__))
#error "PPCSIMBOOKE only compiles on Little Endian x86/x86_64 machines."
#endif

// Define misc macros.
// For debug couts, printfs etc..
#ifndef SIM_DEBUG
#define SIM_DEBUG 1
#endif

// simulator parameters
#ifndef  N_CPUS
#define  N_CPUS                      2     // Number of cpus
#endif

#ifndef  CPU_CACHE_LINE_SIZE
#define  CPU_CACHE_LINE_SIZE         32    // cache line size
#endif

#ifndef  CPU_PHY_ADDR_SIZE
#define  CPU_PHY_ADDR_SIZE           36    // physical address line size
#endif

#ifndef  CPU_TLB4K_N_SETS
#define  CPU_TLB4K_N_SETS            128   // number of sets in tlb4K
#endif

#ifndef  CPU_TLB4K_N_WAYS
#define  CPU_TLB4K_N_WAYS            4     // number of ways/set in tlb4K
#endif

#ifndef  CPU_TLBCAM_N_ENTRIES
#define  CPU_TLBCAM_N_ENTRIES        16    // Number of entries in tlbCam
#endif


#endif
