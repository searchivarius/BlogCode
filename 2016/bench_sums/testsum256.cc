#include <iostream>
#include <typeinfo>
#include <cstdio>
#include "emmintrin.h"
#include "immintrin.h"
#include "../../2014/ztimer.h"

#include <vector>
#include <limits>
#include <algorithm>    // std::min
#if defined(__GNUC__)
#define PORTABLE_ALIGN16 __attribute__((aligned(16)))
#else
#define PORTABLE_ALIGN16 __declspec(align(16))
#endif

inline float mm128_sum1(__m128 reg) {
  float PORTABLE_ALIGN16 TmpRes[4];
  _mm_store_ps(TmpRes, reg);
  return TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3];
};

inline float mm256_sum1(__m256 reg) {
  float PORTABLE_ALIGN16 TmpRes[8];
  _mm256_store_ps(TmpRes, reg);
  return TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3] +
         TmpRes[4] + TmpRes[5] + TmpRes[6] + TmpRes[7];
};

inline float mm256_sum2(__m256 reg) {
  __m128 sum = _mm_add_ps(_mm256_extractf128_ps(reg, 0),
                          _mm256_extractf128_ps(reg, 1));
  return mm128_sum1(sum);
};

using namespace std;

template <float (*sum_func)(__m256)> void do_test(const char* func_name, unsigned N, unsigned repQty) {
  WallClockTimer z;
  uint64_t totalElapsed = 0;
  // The first pass is just to check accuracy
  for (unsigned k = 0; k < N; ++k) {
    float a = k, b = k + 1, c = k+ 2, d = k + 3, e = k + 4, f= k + 5, g= k + 6, h = k + 7;
    float expRes = a + b + c + d + e + f + g + h;
    __m256 inp = _mm256_set_ps(a, b, c, d, e, f, g, h);
    float res = sum_func(inp); 
    if (fabs(res - expRes) > 0.0001) {
      cerr << "Function: " << func_name << " Mismatch: " << res << " vs " << expRes << endl;
      abort();
    }
  }
  // The second pass is to measure efficiency
  float sum_total = 0;
  for (unsigned i = 0; i < repQty; ++i) {
    z.reset();
    for (unsigned k = 0; k < N; ++k) {
      __m256 inp = _mm256_set_ps(k, k + 1, k + 2, k + 3, k + 4, k + 5, k + 6, k + 7);
      float res = sum_func(inp); 
      sum_total += res;
    }
    totalElapsed += z.split();
  }
  cout << "Function: " << func_name << "\tTotal time: " << totalElapsed/1000.0 << " (ms) " << " Ignore: " << sum_total << endl;
};

int main(int argc, char * argv[]) {
  do_test<mm256_sum1>("scalar          ", 1024*1024, 100);
  do_test<mm256_sum2>("part. vectorized", 1024*1024, 100);
  return 0;
};
