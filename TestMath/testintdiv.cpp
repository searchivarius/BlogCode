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

void testDivScalar(size_t N, size_t rep, 
                  uint32_t b1, uint32_t b2, uint32_t b3, uint32_t b4, 
                  uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4) {
    uint32_t sum = 0;
    WallClockTimer timer;

    for(size_t j = 0; j < N; j++){            

        for(size_t i = 0; i < rep; ++i) {
            sum += b1/c1; b1++;
            sum += b2/c2; b2++;
            sum += b3/c3; b3++;
            sum += b4/c4; b4++;
        }
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Integer DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(uint32_t).name() << endl;
    cout << "Milllions of integer DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

void testDivVector(size_t N, size_t rep, 
                   uint32_t b1, uint32_t b2, uint32_t b3, uint32_t b4, 
                   uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4) {
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

            B = _mm_cvtepi32_pd(_mm_set_epi32(0, 0, b4, b3));
            b3++; b4++;
            C = _mm_cvtepi32_pd(_mm_set_epi32(0, 0, c4, c3));
            R = _mm_div_pd(B, C);
            Bi = _mm_cvttpd_epi32(R);
            sum += _mm_extract_epi32(Bi, 0); 
            sum += _mm_extract_epi32(Bi, 1); 
        }
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Integer DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(uint32_t).name() << endl;
    cout << "Milllions of integer DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

void testDivVectorAVX(size_t N, size_t rep, 
                   uint32_t b1, uint32_t b2, uint32_t b3, uint32_t b4, 
                   uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4) {
    uint32_t sum = 0;
    WallClockTimer timer;

    for(size_t j = 0; j < N; j++){            
        for(size_t i = 0; i < rep; ++i) {
            __m256d B = _mm256_cvtepi32_pd(_mm_set_epi32(b4, b3, b2, b1));
            b1++; b2++; b3++; b4++;
            __m256d C = _mm256_cvtepi32_pd(_mm_set_epi32(c4, c3, c2, c1));
            __m256d R = _mm256_div_pd(B, C);
#if 0
            // This conversion function apparently rounds
            // as opposed to doing floor()
            // This is why, if you  uncomment this code,
            // the value of the sum (in the end) will be 
            // slightly different
            __m128i Bi = _mm256_cvtpd_epi32(R);
            sum += _mm_extract_epi32(Bi, 0); 
            sum += _mm_extract_epi32(Bi, 1); 
            sum += _mm_extract_epi32(Bi, 2); 
            sum += _mm_extract_epi32(Bi, 3); 
#else
            __m128d v1 = _mm256_extractf128_pd(R, 0);
            __m128i Bi1 = _mm_cvttpd_epi32(v1);
            sum += _mm_extract_epi32(Bi1, 0); 
            sum += _mm_extract_epi32(Bi1, 1); 
            __m128d v2 = _mm256_extractf128_pd(R, 1);
            __m128i Bi2 = _mm_cvttpd_epi32(v2);
            sum += _mm_extract_epi32(Bi2, 0); 
            sum += _mm_extract_epi32(Bi2, 1); 
#endif
        }
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
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
    const uint32_t MULT = 65536;
#else
    const uint32_t MULT = 256;
#endif
    uint32_t b1=MULT * (rand() % 65536)  + rand() % 65536;
    uint32_t b2=MULT * (rand() % 65536)  + rand() % 65536;
    uint32_t b3=MULT * (rand() % 65536)  + rand() % 65536;
    uint32_t b4=MULT * (rand() % 65536)  + rand() % 65536;
    uint32_t c1=128 * (rand() % 65536)  + rand() % 65536;
    uint32_t c2=128 * (rand() % 65536)  + rand() % 65536;
    uint32_t c3=128 * (rand() % 65536)  + rand() % 65536;
    uint32_t c4=128 * (rand() % 65536)  + rand() % 65536;

    cout << b1 << " -> " << c1 << ": " << b1/c1 << endl;
    cout << b2 << " -> " << c2 << ": " << b2/c2 << endl;
    cout << b3 << " -> " << c3 << ": " << b3/c3 << endl;
    cout << b4 << " -> " << c4 << ": " << b4/c4 << endl;

    testDivScalar(2000000, 16, b1, b2, b3, b4, c1, c2, c3, c4);
    testDivVector(2000000, 16, b1, b2, b3, b4, c1, c2, c3, c4);
    testDivVectorAVX(2000000, 16, b1, b2, b3, b4, c1, c2, c3, c4);

}
