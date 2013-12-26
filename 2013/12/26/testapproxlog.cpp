/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Leonid Boytsov, http://boytsov.info
 */

#include "cmn.h"

using namespace std;

/*
 * Testing fastlog2 from http://fastapprox.googlecode.com/svn/trunk/fastapprox/src/fastonebigheader.h
 * fastlog2 (C) Paul Mineiro <paul@mineiro.com>
 */

static inline float 
fastlog2 (float x)
{
  union { float f; uint32_t i; } vx = { x };
  union { uint32_t i; float f; } mx = { (vx.i & 0x007FFFFF) | 0x3f000000 };
  float y = vx.i;
  y *= 1.1920928955078125e-7f;

  return y - 124.22551499f
           - 1.498030302f * mx.f 
           - 1.72587999f / (0.3520887068f + mx.f);
}

static inline float
fastlog (float x)
{
  return 0.69314718f * fastlog2 (x);
}

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
    cout << "Milllions of Logs per sec: " << (float(TotalQty) / t) << endl;
    
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
    cout << "Milllions of Logs per sec: " << (float(TotalQty) / t) << endl;
    
}

template <class T>
void testFastLogApprox(int N, T start, T end) {
    float rel = 0;
    for (int i = 0; i < N; ++i) {
      T arg = start + RandomReal<T>() * (end - start);
      T val1 = log(arg);
      T val2 = fastlog(arg);

      if (val1 != val2) {
        rel += std::abs(val1 - val2)/std::max(std::abs(val1), std::abs(val2));
      }
    }
    cout << "Relative error: " << rel/ N << " range: [ " << start << " " << end << "]" << endl;
}

int main() {
    SetHighAccuracy();
    testFastLogApprox<float>(10000000, 1e-9, 1);
    testFastLogApprox<float>(10000000, 1, 2);
    testFastLogApprox<float>(10000000, 1, 1000);
    testFastLogApprox<float>(10000000, 1, 1000000);
    testLog<float>(10000, 2000);
    testFastLog<float>(10000, 2000);
}
