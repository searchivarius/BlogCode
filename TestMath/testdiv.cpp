/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Leonid Boytsov, http://boytsov.info
 */

#include "cmn.h"

using namespace std;

#define USE_ONLY_FLOAT true

template <class T>
void testDivMalkovDataDep(size_t N = 210000000, size_t rep = 1) {
// This test was proposed by Yury Malkov and modified by Leonid Boytsov
    T c1=1.0,b1=0.9;        
    T c2=1.0,b2=0.91;        
    T c3=1.0,b3=0.92;  
    T c4=1.0,b4=0.93;  

    WallClockTimer timer;

    T coeff = 1/T(rep) * 0.00001;

    for(size_t j = 0; j < N; j++){            
        for(size_t i = 0; i < rep; ++i) {
            c1+=b1/c1;
            c2+=b2/c2;
            c3+=b3/c3;
            c4+=b4/c4;
        }
        c1 *= coeff;
        c2 *= coeff;
        c3 *= coeff;
        c4 *= coeff;
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    T sum = c1 + c2 + c3 + c4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Test WITH data dependencies" << endl;
    cout << "DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

template <class T>
void testMulMalkovDataDep(size_t N = 210000000, size_t rep = 1) {
// This test was proposed by Yury Malkov and modified by Leonid Boytsov
    T c1=1.0,b1=0.9;        
    T c2=1.0,b2=0.91;        
    T c3=1.0,b3=0.92;  
    T c4=1.0,b4=0.93;  

    WallClockTimer timer;

    T coeff = 1/T(rep) * 0.00001;

    for(size_t j = 0; j < N; j++){            
        for(size_t i = 0; i < rep; ++i) {
            c1+=b1*c1;
            c2+=b2*c2;
            c3+=b3*c3;
            c4+=b4*c4;
        }
        c1 *= coeff;
        c2 *= coeff;
        c3 *= coeff;
        c4 *= coeff;
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    T sum = c1 + c2 + c3 + c4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Test WITH data dependencies" << endl;
    cout << "MULs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of MULs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

template <class T>
void testDivMalkovNoDataDep(size_t N, size_t rep = 1) {
// This test was proposed by Yury Malkov and modified by Leonid Boytsov
    T c1=1.0,b1=0.9;        
    T c2=1.0,b2=0.91;        
    T c3=1.0,b3=0.92;  
    T c4=1.0,b4=0.93;  

    WallClockTimer timer;

    T sum = 0;

    for(size_t i = 0; i < rep; ++i) {
        for(size_t j = 0; j < N; j++){            
            sum+=b1/c1;
            c1 += 0.00001;
            sum+=b2/c2;
            c2 += 0.00001;
            sum+=b3/c3;
            c3 += 0.00001;
            sum+=b4/c4;
            c4 += 0.00001;
        }
        sum /= N;
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Test withOUT data dependencies" << endl;
    cout << "DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

template <class T>
void testMulMalkovNoDataDep(size_t N, size_t rep) {
// This test was proposed by Yury Malkov and modified by Leonid Boytsov
    T c1=1.0,b1=0.9;        
    T c2=1.0,b2=0.91;        
    T c3=1.0,b3=0.92;  
    T c4=1.0,b4=0.93;  

    WallClockTimer timer;

    T sum = 0;

    for(size_t i = 0; i < rep; ++i) {
        for(size_t j = 0; j < N; j++){            
            sum+=b1*c1;
            c1 += 0.00001;
            sum+=b2*c2;
            c2 += 0.00001;
            sum+=b3*c3;
            c3 += 0.00001;
            sum+=b4*c4;
            c4 += 0.00001;
        }
        sum /= N;
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "Test withOUT data dependencies" << endl;
    cout << "MULs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of MULs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
}

template <class T>
void testDiv0(int N, int rep) {
    vector<T>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = 1 + (rand() % 100000000) / 1000.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int i = 0; i < N*4; i+=4) {
      T v1 = data[i];
      T v2 = data[i+1];
      T v3 = data[i+2];
      T v4 = data[i+3];
      for (int j = 0; j < rep; ++j) {
            sum += v1 / v2;
            sum += v2 / v3;
            sum += v3 / v4;
            sum += v4 / v1;
        }
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
    
}

template <class T>
void testMult0(int N, int rep) {
    vector<T>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = 1 + (rand() % 100000000) / 1000.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int i = 0; i < N*4; i+=4) {
        T v1 = data[i];
        T v2 = data[i+1];
        T v3 = data[i+2];
        T v4 = data[i+3];
        for (int j = 0; j < rep; ++j) {
            sum += v1 * v2;
            sum += v2 * v3;
            sum += v3 * v4;
            sum += v4 * v1;
        }
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "MULs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of MULs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
    
}

template <class T>
void testDiv1(int N, int rep) {
    vector<T>   data(N*8);

    for (int i = 0; i < 8*N; ++i) {
        data[i] = 1 + (rand() % 100000000) / 1000.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int i = 0; i < N*8; i+=8) {
        T v1 = data[i];
        T v2 = data[i+1];
        T v3 = data[i+2];
        T v4 = data[i+3];
        T v5 = data[i+4];
        T v6 = data[i+5];
        T v7 = data[i+6];
        T v8 = data[i+7];

        for (int j = 0; j < rep; ++j) {
            sum += v1 / v2;
            v1 += 0.00001;
            sum += v2 / v3;
            v2 += 0.00001;
            sum += v3 / v4;
            v3 += 0.00001;
            sum += v4 / v5;
            v4 += 0.00001;
            sum += v5 / v6;
            v5 += 0.00001;
            sum += v6 / v7;
            v6 += 0.00001;
            sum += v7 / v8;
            v7 += 0.00001;
            sum += v8 / v1;
            v8 += 0.00001;
        }
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 8;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of DIVs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
    
}

template <class T>
void testMult1(int N, int rep) {
    vector<T>   data(N*8);

    for (int i = 0; i < 8*N; ++i) {
        data[i] = 1 + (rand() % 100000000) / 1000.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int i = 0; i < N*8; i+=8) {

      T v1 = data[i];
      T v2 = data[i+1];
      T v3 = data[i+2];
      T v4 = data[i+3];
      T v5 = data[i+4];
      T v6 = data[i+5];
      T v7 = data[i+6];
      T v8 = data[i+7];

      for (int j = 0; j < rep; ++j) {
            sum += v1 * v2;
            v1 += 0.00001;
            sum += v2 * v3;
            v2 += 0.00001;
            sum += v3 * v4;
            v3 += 0.00001;
            sum += v4 * v5;
            v4 += 0.00001;
            sum += v5 * v6;
            v5 += 0.00001;
            sum += v6 * v7;
            v6 += 0.00001;
            sum += v7 * v8;
            v7 += 0.00001;
            sum += v8 * v1;
            v8 += 0.00001;
        }
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 8;
    cout << __func__ << endl;
    cout << "Ignore: " << sum << endl;
    cout << "MULs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of MULs per sec: " << (float(TotalQty) / t) << endl;
    cout << "=============================" << endl;
    
}

int main() {
    SetHighAccuracy();

    testDivMalkovDataDep<float>(100000, 64);
    if (!USE_ONLY_FLOAT) {
      testDivMalkovDataDep<double>(100000, 64);
      testDivMalkovDataDep<long double>(100000, 64);
    }

    testMulMalkovDataDep<float>(100000, 64);
    if (!USE_ONLY_FLOAT) {
      testMulMalkovDataDep<double>(100000, 64);
      testMulMalkovDataDep<long double>(100000, 64);
    }

    testDivMalkovNoDataDep<float>(100000, 64);
    if (!USE_ONLY_FLOAT) {
      testDivMalkovNoDataDep<double>(100000, 64);
      testDivMalkovNoDataDep<long double>(100000, 64);
    }

    testMulMalkovNoDataDep<float>(100000, 64);
    if (!USE_ONLY_FLOAT) {
      testMulMalkovNoDataDep<double>(100000, 64);
      testMulMalkovNoDataDep<long double>(100000, 64);
    }

    testDiv0<float>(100000, 64);
    if (!USE_ONLY_FLOAT) {
      testDiv0<double>(100000, 64);
      testDiv0<long double>(100000, 64);
    }

    testMult0<float>(100000, 64);
    if (!USE_ONLY_FLOAT) {
      testMult0<double>(100000, 64);
      testMult0<long double>(100000, 64);
    }

    testDiv1<float>(10000, 64);
    if (!USE_ONLY_FLOAT) {
      testDiv1<double>(10000, 64);
      testDiv1<long double>(10000, 64);
    }

    testMult1<float>(10000, 64);
    if (!USE_ONLY_FLOAT) {
      testMult1<double>(10000, 64);
      testMult1<long double>(10000, 64);
    }
}
