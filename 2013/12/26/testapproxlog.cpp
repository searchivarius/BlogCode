/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Leonid Boytsov, http://boytsov.info
 */

#include "cmn.h"

#include <vector>

#include "fastlog.h"

using namespace std;

/*
 * Testing fastlog2 and its SIMD version vfastlog2 
 * from http://fastapprox.googlecode.com/svn/trunk/fastapprox/src/
 * fast math functions (C) Paul Mineiro <paul@mineiro.com>
 */

template <class T>
void testLog(int N, int rep) {
    vector<T>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = 1 + RandomReal<T>() * 1000;
    }

    WallClockTimer timer;
    T sum = 0;
    T fract = T(1)/N;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += 0.1 * log(data[i]); 
            sum += 0.1 * log(data[i+1]); 
            sum += 0.1 * log(data[i+2]); 
            sum += 0.1 * log(data[i+3]); 
        }
        sum *= fract;
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Logs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of std::log Logs per sec: " << (float(TotalQty) / t) << endl;
    
}

template <class T>
void testFastLog(int N, int rep) {
    vector<T>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = 1 + RandomReal<T>() * 1000;
    }

    WallClockTimer timer;
    T sum = 0;
    T fract = T(1)/N;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += 0.1 * fastlog(data[i]); 
            sum += 0.1 * fastlog(data[i+1]); 
            sum += 0.1 * fastlog(data[i+2]); 
            sum += 0.1 * fastlog(data[i+3]); 
        }
        sum *= fract;
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Fast Logs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of Fast Logs per sec: " << (float(TotalQty) / t) << endl;
    
}

void testSIMDFastLog(int N, int rep) {
    vector<float>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = 1 + RandomReal<float>() * 1000;
    }

    WallClockTimer timer;
    float  sum = 0;
    v4sf   sumSIMD = _mm_set1_ps(0);
    float  fract = float(1)/N;
    v4sf   mul = _mm_set1_ps(0.1);

    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sumSIMD = _mm_add_ps(sumSIMD, 
                                _mm_mul_ps( 
                                           vfastlog2(_mm_loadu_ps(&data[i])), 
                                           mul)
                                );
        }
        float tmp[4];
        _mm_storeu_ps(tmp, sumSIMD);
        sum += fract * (tmp[0] + tmp[1] + tmp[2] + tmp[3]);
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Fast SIMD Logs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, float type" << endl;
    cout << "Milllions of SIMD Fast Logs per sec: " << (float(TotalQty) / t) << endl;
    
}

template <class T>
void compDiscrLogTable(size_t discrQty,
                      T start, T end,
                      vector<T>& table,
                      T& inverseD) {
    table.resize(discrQty + 1);

    T D = (end - start)/discrQty;
    inverseD = discrQty / (end - start);
    for (int i = 0; i <= discrQty; ++i) {
      table [i] = log(start + D * i + D/2); 
    }

};


void testDiscreteSIMDLog(int N, int rep, float start, float end, size_t discrQty) {
    vector<float>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = start + RandomReal<float>() * (end - start);
    }

    vector<float> table;
    float         inverseD;

    compDiscrLogTable<float>(discrQty, 1, 2, table, inverseD);

    WallClockTimer timer;
    float  sum = 0;
    v4sf   sumSIMD = _mm_set1_ps(0);
    float  fract = float(1)/N;
    v4sf   mul = _mm_set1_ps(0.1);

    v4sf cmult = _mm_set1_ps(inverseD);
    v4sf csub  = _mm_set1_ps(start);
    v4si tmpi; 
    int TmpRes[4];

    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
          tmpi    = _mm_cvttps_epi32(_mm_mul_ps(cmult, 
                                                     // subtract start so that the range is from star to end
                                                      _mm_sub_ps(_mm_loadu_ps(&data[i]), csub)
                                               )
                                    );
          _mm_store_si128(reinterpret_cast<__m128i*>(TmpRes), tmpi);
          sumSIMD = _mm_add_ps(sumSIMD, 
                              _mm_set_ps(table[TmpRes[3]], table[TmpRes[2]], table[TmpRes[1]], table[TmpRes[0]]));
          
        }
        float tmp[4];
        _mm_storeu_ps(tmp, sumSIMD);
        sum += fract * (tmp[0] + tmp[1] + tmp[2] + tmp[3]);
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Fast SIMD Approx Logs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, float type" << endl;
    cout << "Milllions of SIMD Approx Fast Logs per sec: [" << start << " " << end << "] " 
         << (float(TotalQty) / t) << " Table size: " << discrQty << endl;
    
}

template <class T>
void testDiscreteLog(int N, int rep, T start, T end, size_t discrQty) {
    vector<T> table;
    T         inverseD;

    compDiscrLogTable<T>(discrQty, start, end, table, inverseD);

    vector<T>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = start + RandomReal<T>() * (end - start);
    }

    #define COMP(arg)  table[size_t((arg - start) * inverseD)];

    WallClockTimer timer;
    T sum = 0;
    T fract = T(1)/N;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += 0.1 * COMP(data[i]); 
            sum += 0.1 * COMP(data[i+1]); 
            sum += 0.1 * COMP(data[i+2]); 
            sum += 0.1 * COMP(data[i+3]); 
        }
        sum *= fract;
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Discrete logs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of discrete Logs [" << start << " " << end << "]  per sec: " << (float(TotalQty) / t) << " Table size: " << discrQty << endl;
    
}

template <class T>
void testFastLogApprox(int N, T start, T end) {
    T rel = 0;
    for (int i = 0; i < N; ++i) {
      T arg = start + RandomReal<T>() * (end - start);
      T val1 = log(arg);
      T val2 = fastlog(arg);

      if (val1 != val2) {
        rel += std::abs(val1 - val2)/std::max(std::abs(val1), std::abs(val2));
      }
    }
    cout << "Fast logs, relative error: " << rel/ N << " range: [ " << start << " " << end << "]" << endl;
}

template <class T>
void testDiscreteLogApprox(int N, T start, T end, size_t discrQty) {
    vector<T> table(discrQty);

    T D = (end - start)/discrQty;
    T inverseD = discrQty / (end - start);
    for (int i = 0; i <= discrQty; ++i) {
      table [i] = log(start + D * i + D/2); 
    }

    T rel = 0;
    for (int i = 0; i < N; ++i) {
      T arg = start + RandomReal<T>() * (end - start);
      T val1 = log(arg);
      T val2 = table[size_t((arg - start) * inverseD)];

      if (val1 != val2) {
        rel += std::abs(val1 - val2)/std::max(std::abs(val1), std::abs(val2));
      }
    }
    cout << "Discrete logs, table size: " << discrQty << " relative error: " << rel/ N << " range: [ " << start << " " << end << "]" << endl;
}

int main() {
    SetHighAccuracy();
    testFastLogApprox<float>(10000000, 1e-9, 1);
    testFastLogApprox<float>(10000000, 1, 2);
    testFastLogApprox<float>(10000000, 1, 1000);
    testFastLogApprox<float>(10000000, 1, 1000000);
    testDiscreteSIMDLog(10000, 2000, 1, 2, 65536 * 2);
    testDiscreteSIMDLog(10000, 2000, 1, 2, 65536);
    testDiscreteSIMDLog(10000, 2000, 1, 2, 32768);
    testDiscreteSIMDLog(10000, 2000, 1, 2, 16536);
    testDiscreteSIMDLog(10000, 2000, 1, 2, 4096);
    testDiscreteSIMDLog(10000, 2000, 1, 2, 1024);
    testDiscreteLogApprox<float>(10000000, 1, 2, 65536 * 2);
    testDiscreteLogApprox<float>(10000000, 1, 2, 65536);
    testDiscreteLogApprox<float>(10000000, 1, 2, 32768);
    testDiscreteLogApprox<float>(10000000, 1, 2, 16536);
    testDiscreteLogApprox<float>(10000000, 1, 2, 4096);
    testDiscreteLogApprox<float>(10000000, 1, 2, 1024);
    testLog<float>(10000, 2000);
    testFastLog<float>(10000, 2000);
    testSIMDFastLog(10000, 2000);
    testDiscreteLog<float>(10000, 2000, 1, 2, 65536 * 2);
    testDiscreteLog<float>(10000, 2000, 1, 2, 65536);
    testDiscreteLog<float>(10000, 2000, 1, 2, 32768);
    testDiscreteLog<float>(10000, 2000, 1, 2, 16536);
    testDiscreteLog<float>(10000, 2000, 1, 2, 4096);
    testDiscreteLog<float>(10000, 2000, 1, 2, 1024);
}
