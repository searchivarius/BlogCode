#ifdef __SSE4_2__
#include <immintrin.h>
#include <smmintrin.h>
#include <tmmintrin.h>
#else
#error "SSE 4.2 is required!"
#endif

#include <iostream>

 /*
 * Based on explanations/suggestions from here
 * http://stackoverflow.com/questions/5526658/intel-sse-why-does-mm-extract-ps-return-int-instead-of-float
 */

#define MM_EXTRACT_DOUBLE(v,i) _mm_cvtsd_f64(_mm_shuffle_pd(v, v, _MM_SHUFFLE2(0, i)))

using namespace std;

int main(int,char**) {
  cout << "Extracting two double-precision floating-point elements:" << endl;
  __m128d doubleVar = _mm_set_pd(0.2, 0.1);

  cout << MM_EXTRACT_DOUBLE(doubleVar, 0) << endl;
  cout << MM_EXTRACT_DOUBLE(doubleVar, 1) << endl;

  return 0;
}
