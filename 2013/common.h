/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * Originally put together by:
 * Daniel Lemire, http://lemire.me/en/
 *
 * modified by:
 * Leonid Boytsov, http://boytsov.info
 */
#ifndef COMMON_H_
#define COMMON_H_

// C headers (sorted)
#include <errno.h>
#include <fcntl.h>
#if defined __SSE2__
#include <immintrin.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>
#endif
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

// C++ headers (sorted)
#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>


#endif /* COMMON_H_ */
