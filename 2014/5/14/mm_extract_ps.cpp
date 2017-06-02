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

#define MM_EXTRACT_FLOAT(v,i)  _mm_cvtss_f32(_mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, i)))

using namespace std;

int main(int,char**) {
  cout << "Extracting four single-precision floating-point elements:" << endl;
  __m128 floatVar = _mm_set_ps(0.4, 0.3, 0.2, 0.1);

  cout << MM_EXTRACT_FLOAT(floatVar, 0) << endl;
  cout << MM_EXTRACT_FLOAT(floatVar, 1) << endl;
  cout << MM_EXTRACT_FLOAT(floatVar, 2) << endl;
  cout << MM_EXTRACT_FLOAT(floatVar, 3) << endl;

  return 0;
}
