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
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

// Include all common misc local header files
#include "exception.hpp"
#include "ppc_exception.h"
#include "log.hpp"
#include "utils.h"

#include "misc.hpp"

// Define misc macros.
// For debug couts, printfs etc..
#define SIM_DEBUG 1

#endif
