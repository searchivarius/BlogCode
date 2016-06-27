#include <iostream>
#include <typeinfo>
#include <cstdio>
#include "emmintrin.h"
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

inline float mm128_sum2(__m128 reg) {
  __m128 tmp = _mm_hadd_ps(reg, reg);
  return _mm_cvtss_f32(_mm_hadd_ps(tmp, tmp));
};

inline float mm128_sum3(__m128 regorig) {
  __m128i reg = _mm_cvtps_epi32(regorig);
  return 
    _mm_cvtss_f32(regorig) + 
    _mm_cvtss_f32(_mm_cvtepi32_ps(_mm_shuffle_epi32(reg, _MM_SHUFFLE(0, 0, 0, 1)))) + 
    _mm_cvtss_f32(_mm_cvtepi32_ps(_mm_shuffle_epi32(reg, _MM_SHUFFLE(0, 0, 0, 2)))) + 
    _mm_cvtss_f32(_mm_cvtepi32_ps(_mm_shuffle_epi32(reg, _MM_SHUFFLE(0, 0, 0, 3))));

};

using namespace std;

template <float (*sum_func)(__m128)> void do_test(unsigned N, unsigned repQty) {
  WallClockTimer z;
  uint64_t totalElapsed = 0;
  // The first pass is just to check accuracy
  for (unsigned k = 0; k < N; ++k) {
    float a = k, b = k + 1, c = k+ 2, d = k + 3;
    float expRes = a + b + c + d;
    __m128 inp = _mm_set_ps(a, b, c, d);
    float res = sum_func(inp); 
    if (fabs(res - expRes) > 0.0001) {
      cerr << "Function: " << typeid(sum_func).name() << " Mismatch: " << res << " vs " << expRes << endl;
      abort();
    }
  }
  // The second pass is to measure efficiency
  float sum_total = 0;
  for (unsigned i = 0; i < repQty; ++i) {
    z.reset();
    for (unsigned k = 0; k < N; ++k) {
      __m128 inp = _mm_set_ps(k, k + 1, k + 2, k + 3);
      float res = sum_func(inp); 
      sum_total += res;
    }
    totalElapsed += z.split();
  }
  cout << "Function: " << typeid(sum_func).name() << " Ignore: " << sum_total << "Total time: " << totalElapsed/1000.0 << " (ms) " << endl;
};

int main(int argc, char * argv[]) {
  do_test<mm128_sum1>(1024*1024, 100);
  do_test<mm128_sum2>(1024*1024, 100);
  do_test<mm128_sum3>(1024*1024, 100);
  return 0;
};
