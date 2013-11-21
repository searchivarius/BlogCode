/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Leonid Boytsov, http://boytsov.info
 * 
 * Implementing ideas of Nathan Kurz: let's vectorize integer division
 * by converting integers to floating-point numbers
 */

#include "cmn.h"

#if defined __i386__ || defined __x86_64__
#include <immintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>
#endif

#if defined __x86_64__
#include "vectori128.h"
#endif

using namespace std;

void testDiv16Scalar(size_t N, size_t rep, 
                  uint16_t b1, uint16_t b2, uint16_t b3, uint16_t b4, 
                  uint16_t b5, uint16_t b6, uint16_t b7, uint16_t b8, 
                  uint16_t c1, uint16_t c2, uint16_t c3, uint16_t c4,
                  uint16_t c5, uint16_t c6, uint16_t c7, uint16_t c8
) {
    uint32_t sum = 0;
    WallClockTimer timer;

    for(size_t j = 0; j < N; j++){            

        for(size_t i = 0; i < rep; ++i) {
            sum += b1/c1; b1++;
            sum += b2/c2; b2++;
            sum += b3/c3; b3++;
            sum += b4/c4; b4++;
            sum += b5/c5; b5++;
            sum += b6/c6; b6++;
            sum += b7/c7; b7++;
            sum += b8/c8; b8++;
        }
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 8;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "16-bit Integer DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(uint16_t).name() << endl;
    cout << "Milllions of 16-bit integer DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

void testDiv16VectorFloat(size_t N, size_t rep, 
                  uint16_t b1, uint16_t b2, uint16_t b3, uint16_t b4, 
                  uint16_t b5, uint16_t b6, uint16_t b7, uint16_t b8, 
                  uint16_t c1, uint16_t c2, uint16_t c3, uint16_t c4,
                  uint16_t c5, uint16_t c6, uint16_t c7, uint16_t c8) {
    uint32_t sum = 0;
    WallClockTimer timer;

#if defined __i386__ || defined __x86_64__
    __m128  B, C, R;
    __m128i Bi;

    for(size_t j = 0; j < N; j++){            
        for(size_t i = 0; i < rep; ++i) {
            B = _mm_cvtepi32_ps(_mm_set_epi32(b4, b3, b2, b1));
            b1++; b2++; b3++; b4++;
            C = _mm_cvtepi32_ps(_mm_set_epi32(c4, c3, c2, c1));
            R = _mm_div_ps(B, C);
            Bi = _mm_cvttps_epi32(R);
            sum += _mm_extract_epi32(Bi, 0); 
            sum += _mm_extract_epi32(Bi, 1); 
            sum += _mm_extract_epi32(Bi, 2); 
            sum += _mm_extract_epi32(Bi, 3); 

            B = _mm_cvtepi32_ps(_mm_set_epi32(b8, b7, b6, b5));
            b5++; b6++; b7++; b8++;
            C = _mm_cvtepi32_ps(_mm_set_epi32(c8, c7, c6, c5));
            R = _mm_div_ps(B, C);
            Bi = _mm_cvttps_epi32(R);
            sum += _mm_extract_epi32(Bi, 0); 
            sum += _mm_extract_epi32(Bi, 1); 
            sum += _mm_extract_epi32(Bi, 2); 
            sum += _mm_extract_epi32(Bi, 3); 
        }
    }
#endif

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 8;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Integer 16-bit DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(uint32_t).name() << endl;
    cout << "Milllions of 16-bit integer DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

void testDiv16VectorFloatAvx(size_t N, size_t rep, 
                  uint16_t b1, uint16_t b2, uint16_t b3, uint16_t b4, 
                  uint16_t b5, uint16_t b6, uint16_t b7, uint16_t b8, 
                  uint16_t c1, uint16_t c2, uint16_t c3, uint16_t c4,
                  uint16_t c5, uint16_t c6, uint16_t c7, uint16_t c8) {
    uint32_t sum = 0;
    WallClockTimer timer;


#if defined __i386__ || defined __x86_64__
    for(size_t j = 0; j < N; j++){            
        for(size_t i = 0; i < rep; ++i) {
            __m256 B = _mm256_cvtepi32_ps(_mm256_set_epi32(b8, b7, b6, b5, b4, b3, b2, b1));
            b1++; b2++; b3++; b4++; b5++; b6++; b7++; b8++;
            __m256 C = _mm256_cvtepi32_ps(_mm256_set_epi32(c8, c7, c6, c5, c4, c3, c2, c1));
            __m256 R = _mm256_div_ps(B, C);

            __m128 v1 = _mm256_extractf128_ps(R, 0);
            __m128i Bi1 = _mm_cvttps_epi32(v1);
            sum += _mm_extract_epi32(Bi1, 0); 
            sum += _mm_extract_epi32(Bi1, 1); 
            sum += _mm_extract_epi32(Bi1, 2); 
            sum += _mm_extract_epi32(Bi1, 3); 

            __m128 v2 = _mm256_extractf128_ps(R, 1);
            __m128i Bi2 = _mm_cvttps_epi32(v2);
            sum += _mm_extract_epi32(Bi2, 0); 
            sum += _mm_extract_epi32(Bi2, 1); 
            sum += _mm_extract_epi32(Bi2, 2); 
            sum += _mm_extract_epi32(Bi2, 3); 
        }
    }
#endif

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 8;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Integer 16-bit DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(uint32_t).name() << endl;
    cout << "Milllions of 16-bit integer DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

/*
 * This function uses files from the Agner's vector class library:
 * http://www.agner.org/optimize/#vectorclass
 *
 * These files are distributed under the GNU license v 3, for details see:
 * http://www.gnu.org/licenses/quick-guide-gplv3.html 
 *
 */
void testDiv32AgnerOneDiv(size_t N, size_t rep, 
                  uint32_t b1, uint32_t b2, uint32_t b3, uint32_t b4, 
                  uint32_t c) {
    uint32_t sum = 0;
    WallClockTimer timer;

#if defined __i386__ || defined __x86_64__
    for(size_t j = 0; j < N; j++){            
        for(size_t i = 0; i < rep; ++i) {
            Vec4ui   B(b4, b3, b2, b1);
            Vec4ui   D = B / c;
            b1++; b2++; b3++; b4++;
            sum += D[0] + D[1] + D[2] + D[3];
        }
    }
#endif

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "32-bit Integer DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(uint32_t).name() << endl;
    cout << "Milllions of 32-bit integer DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

void testDiv32ScalarOneDiv(size_t N, size_t rep, 
                  uint32_t b1, uint32_t b2, uint32_t b3, uint32_t b4, 
                  uint32_t c) {
    uint32_t sum = 0;
    WallClockTimer timer;

    for(size_t j = 0; j < N; j++){            

        for(size_t i = 0; i < rep; ++i) {
            sum += b1/c; b1++;
            sum += b2/c; b2++;
            sum += b3/c; b3++;
            sum += b4/c; b4++;
        }
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "32-bit Integer DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(uint32_t).name() << endl;
    cout << "Milllions of 32-bit integer DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

void testDiv32Scalar(size_t N, size_t rep, 
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
    cout << "32-bit Integer DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(uint32_t).name() << endl;
    cout << "Milllions of 32-bit integer DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

void testDiv32VectorDouble(size_t N, size_t rep, 
                   uint32_t b1, uint32_t b2, uint32_t b3, uint32_t b4, 
                   uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4) {
    uint32_t sum = 0;
    WallClockTimer timer;

#if defined __i386__ || defined __x86_64__
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
#endif

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Integer DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(uint32_t).name() << endl;
    cout << "Milllions of integer DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

void testDiv32VectorAVXDouble(size_t N, size_t rep, 
                   uint32_t b1, uint32_t b2, uint32_t b3, uint32_t b4, 
                   uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4) {
    uint32_t sum = 0;
    WallClockTimer timer;

#if defined __i386__ || defined __x86_64__
    for(size_t j = 0; j < N; j++){            
        for(size_t i = 0; i < rep; ++i) {
            __m256d B = _mm256_cvtepi32_pd(_mm_set_epi32(b4, b3, b2, b1));
            b1++; b2++; b3++; b4++;
            __m256d C = _mm256_cvtepi32_pd(_mm_set_epi32(c4, c3, c2, c1));
            __m256d R = _mm256_div_pd(B, C);

            __m128i Bi = _mm256_cvttpd_epi32(R);
            sum += _mm_extract_epi32(Bi, 0); 
            sum += _mm_extract_epi32(Bi, 1); 
            sum += _mm_extract_epi32(Bi, 2); 
            sum += _mm_extract_epi32(Bi, 3); 
        }
    }
#endif

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Integer DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(uint32_t).name() << endl;
    cout << "Milllions of integer DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

#ifdef __INTEL_COMPILER
void testDiv32VectorIntel(size_t N, size_t rep, 
                   uint32_t b1, uint32_t b2, uint32_t b3, uint32_t b4, 
                   uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4) {
    uint32_t sum = 0;
    WallClockTimer timer;

   __m128i C = _mm_set_epi32(c4, c3, c2, c1);

    for(size_t j = 0; j < N; j++){            
        for(size_t i = 0; i < rep; ++i) {
            __m128i B = _mm_set_epi32(b4, b3, b2, b1);
            b1++; b2++; b3++; b4++;
            __m128i R = _mm_div_epi32(B, C);

            sum += _mm_extract_epi32(R, 0); 
            sum += _mm_extract_epi32(R, 1); 
            sum += _mm_extract_epi32(R, 2); 
            sum += _mm_extract_epi32(R, 3); 
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
#endif

void TestSmallNum() {
/* 
 * A catch: doesn't work with large unsigned integers,
 * because all conversion routines assume integers being signed 
 */
    const uint32_t MULT= 128;

    uint32_t b1=MULT * (rand() % 256)  + rand() % 256;
    uint32_t b2=MULT * (rand() % 256)  + rand() % 256;
    uint32_t b3=MULT * (rand() % 256)  + rand() % 256;
    uint32_t b4=MULT * (rand() % 256)  + rand() % 256;
    uint32_t b5=MULT * (rand() % 256)  + rand() % 256;
    uint32_t b6=MULT * (rand() % 256)  + rand() % 256;
    uint32_t b7=MULT * (rand() % 256)  + rand() % 256;
    uint32_t b8=MULT * (rand() % 256)  + rand() % 256;

    uint32_t c1=4 * (rand() % 256)  + rand() % 256;
    uint32_t c2=4 * (rand() % 256)  + rand() % 256;
    uint32_t c3=4 * (rand() % 256)  + rand() % 256;
    uint32_t c4=4 * (rand() % 256)  + rand() % 256;
    uint32_t c5=4 * (rand() % 256)  + rand() % 256;
    uint32_t c6=4 * (rand() % 256)  + rand() % 256;
    uint32_t c7=4 * (rand() % 256)  + rand() % 256;
    uint32_t c8=4 * (rand() % 256)  + rand() % 256;

    cout << b1 << " -> " << c1 << ": " << b1/c1 << endl;
    cout << b2 << " -> " << c2 << ": " << b2/c2 << endl;
    cout << b3 << " -> " << c3 << ": " << b3/c3 << endl;
    cout << b4 << " -> " << c4 << ": " << b4/c4 << endl;
    cout << b5 << " -> " << c5 << ": " << b5/c5 << endl;
    cout << b6 << " -> " << c6 << ": " << b6/c6 << endl;
    cout << b7 << " -> " << c7 << ": " << b7/c7 << endl;
    cout << b8 << " -> " << c8 << ": " << b8/c8 << endl;

    testDiv16Scalar(2000000, 16, b1, b2, b3, b4, b5, b6, b7, b8,
                                 c1, c2, c3, c4, c5, c6, c7, c8);
    testDiv16VectorFloat(2000000, 16, b1, b2, b3, b4, b5, b6, b7, b8,
                                c1, c2, c3, c4, c5, c6, c7, c8);
    testDiv16VectorFloatAvx(2000000, 16, b1, b2, b3, b4, b5, b6, b7, b8,
                                c1, c2, c3, c4, c5, c6, c7, c8);
}

void TestLargeNum() {
/* 
 * A catch: doesn't work with large unsigned integers,
 * because all conversion routines assume integers being signed 
 */
#if 0
    const uint32_t MULT= 65536;
#else
    const uint32_t MULT= 32768;
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

    testDiv32Scalar(2000000, 16, b1, b2, b3, b4, c1, c2, c3, c4);
#ifdef __INTEL_COMPILER
    testDiv32VectorIntel(2000000, 16, b1, b2, b3, b4, c1, c2, c3, c4);
#endif
    testDiv32VectorDouble(2000000, 16, b1, b2, b3, b4, c1, c2, c3, c4);
    testDiv32VectorAVXDouble(2000000, 16, b1, b2, b3, b4, c1, c2, c3, c4);

    
    testDiv32ScalarOneDiv(2000000, 16, b1, b2, b3, b4, c1);
    testDiv32AgnerOneDiv(2000000, 16, b1, b2, b3, b4, c1);
}

int main() {
    SetHighAccuracy();

    TestSmallNum();
    TestLargeNum();
}
