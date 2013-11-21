/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Leonid Boytsov, http://boytsov.info
 */

#include "cmn.h"

#if defined __SSE2__
#include <immintrin.h>
#endif

using namespace std;

#define USE_ONLY_FLOAT false

/*
 * I thank Yury Malkov for the ideas and scrutiny of my tests.
 * In particular, for the discussion on how data dependencies and
 * compiler optimizations may affect the outcome.
 *
 * The recommended make command: make -f Makefile.gcc_Ofast testdiv
 */

template <class T>
void testDivDataDep0(size_t N, size_t rep) {
    T sum = 0;
    WallClockTimer timer;

    for(size_t j = 0; j < N; j++){            
        T c1=1.0,b1=0.9;        
        T c2=1.0,b2=0.91;        
        T c3=1.0,b3=0.92;  
        T c4=1.0,b4=0.93;  

        for(size_t i = 0; i < rep; ++i) {
            c1+=b1/c4;
            c2+=b2/c1;
            c3+=b3/c2;
            c4+=b4/c3;
        }
        sum += c1 + c2 + c3 + c4;
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Test WITH data dependencies" << endl;
    cout << "DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

template <class T>
void testDivDataDep1(size_t N, size_t rep) {
    T sum = 0;
    WallClockTimer timer;

    for(size_t j = 0; j < N; j++){            
        T c1=1.0,b1=0.9;        
        T c2=1.0,b2=0.91;        
        T c3=1.0,b3=0.92;  
        T c4=1.0,b4=0.93;  

        for(size_t i = 0; i < rep; ++i) {
            c1+=b1/c1;
            c2+=b2/c2;
            c3+=b3/c3;
            c4+=b4/c4;
        }
        sum += c1 + c2 + c3 + c4;
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Test WITH data dependencies" << endl;
    cout << "DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

template <class T>
void testMulDataDep0(size_t N, size_t rep) {
    T sum = 0;

    WallClockTimer timer;

    for(size_t j = 0; j < N; j++){            
        T c1=1.0,b1=0.9;        
        T c2=1.0,b2=0.91;        
        T c3=1.0,b3=0.92;  
        T c4=1.0,b4=0.93;  

        for(size_t i = 0; i < rep; ++i) {
            c1+=b1*c4;
            c2+=b2*c1;
            c3+=b3*c2;
            c4+=b4*c3;
        }
        sum += c1 + c2 + c3 + c4;
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Test WITH data dependencies" << endl;
    cout << "MULs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of MULs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

template <class T>
void testMulDataDep1(size_t N, size_t rep) {
    T sum = 0;
    WallClockTimer timer;

    for(size_t j = 0; j < N; j++){            
        T c1=1.0,b1=0.9;        
        T c2=1.0,b2=0.91;        
        T c3=1.0,b3=0.92;  
        T c4=1.0,b4=0.93;  

        for(size_t i = 0; i < rep; ++i) {
            c1+=b1*c1;
            c2+=b2*c2;
            c3+=b3*c3;
            c4+=b4*c4;
        }
        sum += c1 + c2 + c3 + c4;
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Test WITH data dependencies" << endl;
    cout << "MULs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of MULs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

template <class T>
void testDiv2AtOnce(size_t N, size_t rep) {
    T sum = 0;

    WallClockTimer timer;

    T coeff = 1/T(rep) * 0.00001;

    for(size_t j = 0; j < N; j++){            
        T c1=1.0,b1=0.9;        
        T c2=1.0,b2=0.91;        
        T c3=1.0,b3=0.92;  
        T c4=1.0,b4=0.93;  

        for(size_t i = 0; i < rep; ++i) {
            T r12 = T(1.0)/ (c1 * c2);
            T r34 = T(1.0)/ (c3 * c4);

#if 0
/* 
 * This version actually works a bit faster for single-precision numbers.
 * There is an advantage of using it, though, with double and long double.
 *
 * An old optimization trick, compute two divs at once:
 * http://stereopsis.com/2div.html
 * doesn't work any more.
 */
            c1+=b1/c1;
            c2+=b2/c2;
            c3+=b3/c3;
            c4+=b4/c4;
#else
/*
// Just to double-check I'm doing the right thing
cout << b1/c1 << " -> " << b1 * c2 * r12 << endl;
cout << b2/c2 << " -> " << b2 * c1 * r12 << endl;
cout << b3/c3 << " -> " << b3 * c4 * r34 << endl;
cout << b4/c4 << " -> " << b4 * c3 * r34 << endl;
*/
            c1+=b1 * c2 * r12;
            c2+=b2 * c1 * r12;
            c3+=b3 * c4 * r34;
            c4+=b4 * c3 * r34;
#endif
        }
        sum += c1 + c2 + c3 + c4;
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Test WITH data dependencies" << endl;
    cout << "DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

void testDivDataDep1_SIMD(size_t N, size_t rep) {
    float sum = 0;
    WallClockTimer timer;

#if defined __SSE2__
    for(size_t j = 0; j < N; j++){            
        __m128 b = _mm_set_ps(0.9, 0.91, 0.92, 0.93);
        __m128 c = _mm_set_ps(1, 1, 1, 1);

        size_t  i = 0;
        for(; i < rep/4*4; i += 4) {
          c = _mm_add_ps(c, _mm_div_ps(b, c));
          c = _mm_add_ps(c, _mm_div_ps(b, c));
          c = _mm_add_ps(c, _mm_div_ps(b, c));
          c = _mm_add_ps(c, _mm_div_ps(b, c));
        }
        for(; i < rep; ++i) {
          c = _mm_add_ps(c, _mm_div_ps(b, c));
        }
        float __attribute__((aligned(16))) TmpRes[4];

        _mm_store_ps(TmpRes, c);

        sum += TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3];
    }
#endif

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Test WITH data dependencies" << endl;
    cout << "DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(float).name() << endl;
    cout << "Milllions of DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

void testMulDataDep1_SIMD(size_t N, size_t rep) {
    float sum = 0;
    WallClockTimer timer;

#if defined __SSE2__
    for(size_t j = 0; j < N; j++){            
        __m128 b = _mm_set_ps(0.9, 0.91, 0.92, 0.93);
        __m128 c = _mm_set_ps(1, 1, 1, 1);

        size_t  i = 0;
        for(; i < rep/4*4; i += 4) {
          c = _mm_add_ps(c, _mm_mul_ps(b, c));
          c = _mm_add_ps(c, _mm_mul_ps(b, c));
          c = _mm_add_ps(c, _mm_mul_ps(b, c));
          c = _mm_add_ps(c, _mm_mul_ps(b, c));
        }
        for(; i < rep; ++i) {
          c = _mm_add_ps(c, _mm_div_ps(b, c));
        }
        float __attribute__((aligned(16))) TmpRes[4];

        _mm_store_ps(TmpRes, c);

        sum += TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3];
    }
#endif

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Test WITH data dependencies" << endl;
    cout << "DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(float).name() << endl;
    cout << "Milllions of DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

int main() {
    SetHighAccuracy();

    testDivDataDep0<float>(100000, 64);
    if (!USE_ONLY_FLOAT) {
      testDivDataDep0<double>(100000, 64);
      testDivDataDep0<long double>(100000, 64);
    }

    testDivDataDep1<float>(100000, 64);
    if (!USE_ONLY_FLOAT) {
      testDivDataDep1<double>(100000, 64);
      testDivDataDep1<long double>(100000, 64);
    }

    testDivDataDep1_SIMD(100000, 64);

    testDiv2AtOnce<float>(100000, 64);
    if (!USE_ONLY_FLOAT) {
      testDiv2AtOnce<double>(100000, 64);
      testDiv2AtOnce<long double>(100000, 64);
    }

    testMulDataDep0<float>(100000, 64);
    if (!USE_ONLY_FLOAT) {
      testMulDataDep0<double>(100000, 64);
      testMulDataDep0<long double>(100000, 64);
    }

    testMulDataDep1<float>(100000, 64);
    if (!USE_ONLY_FLOAT) {
      testMulDataDep1<double>(100000, 64);
      testMulDataDep1<long double>(100000, 64);
    }

    testMulDataDep1_SIMD(100000, 64);
}
