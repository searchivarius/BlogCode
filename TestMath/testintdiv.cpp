/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Leonid Boytsov, http://boytsov.info
 */

#include "cmn.h"

#include <immintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>

using namespace std;

void testDivScalar(size_t N, size_t rep, uint32_t b1, uint32_t b2, uint32_t c1, uint32_t c2) {
    uint32_t sum = 0;
    WallClockTimer timer;

    for(size_t j = 0; j < N; j++){            

        for(size_t i = 0; i < rep; ++i) {
            sum += b1/c1; b1++;
            sum += b2/c2; b2++;
        }
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 2;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Integer DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(uint32_t).name() << endl;
    cout << "Milllions of integer DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

void testDivVector(size_t N, size_t rep, uint32_t b1, uint32_t b2, uint32_t c1, uint32_t c2) {
    uint32_t sum = 0;
    WallClockTimer timer;

    __m128d B, C, R;
    __m128i Bi;

    for(size_t j = 0; j < N; j++){            

        for(size_t i = 0; i < rep; ++i) {
            B = _mm_cvtepi32_pd(_mm_set_epi32(0, 0, b2, b1));
            b1++; b2++;
            C = _mm_cvtepi32_pd(_mm_set_epi32(0, 0, c2, c1));
            R = _mm_div_pd(B, C);
            Bi = _mm_cvttpd_epi32(R);
            sum += _mm_extract_epi32(Bi, 0); 
            sum += _mm_extract_epi32(Bi, 1); 
        }
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 2;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Integer DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(uint32_t).name() << endl;
    cout << "Milllions of integer DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

int main() {
    SetHighAccuracy();

    
/* 
 * A catch: doesn't work with large unsigned integers,
 * because all conversion routines assume integers being signed 
 */
#if 0
    uint32_t b1=65536 * (rand() % 65536)  + rand() % 65536;
    uint32_t b2=65536 * (rand() % 65536)  + rand() % 65536;
#else
    uint32_t b1=32768 * (rand() % 65536)  + rand() % 65536;
    uint32_t b2=32768 * (rand() % 65536)  + rand() % 65536;
#endif
    uint32_t c1=128 * (rand() % 65536)  + rand() % 65536;
    uint32_t c2=128 * (rand() % 65536)  + rand() % 65536;

    cout << b1 << " -> " << c1 << endl;

    testDivScalar(2000000, 16, b1, b2, c1, c2);
    testDivVector(2000000, 16, b1, b2, c1, c2);
}
