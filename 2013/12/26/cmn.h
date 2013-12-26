#ifndef CMN_H
#define CMN_H

#include <stdexcept>
#include <iostream>
#include <random>

#ifdef  __INTEL_COMPILER
#include "mkl.h"
#endif

#include "../ztimer.h"

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

template <class T>
inline T RandomReal() {
    // Static is thread-safe in C++ 11
    static std::random_device rdev;
    static std::mt19937 gen(rdev());
    static std::uniform_real_distribution<T> distr(0, 1);

    return distr(gen);
}


#endif
