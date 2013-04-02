#ifndef CMN_H
#define CMN_H

#include <stdexcept>
#include <iostream>

#ifdef  __INTEL_COMPILER
#include "mkl.h"
#endif

#include "ztimer.h"

inline void SetHighAccuracy() {
#ifdef  __INTEL_COMPILER
    vmlSetMode(VML_HA);
    std::cout << "Set high-accuracy mode." << std::endl;
#endif
}

inline void SetLowAccuracy() {
#ifdef  __INTEL_COMPILER
    vmlSetMode(VML_LA);
    std::cout << "Set low-accuracy mode." << std::endl;
#endif
}

#endif
