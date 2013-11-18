/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Leonid Boytsov, http://boytsov.info
 */

#include "cmn.h"

using namespace std;

#define USE_ONLY_FLOAT true

/*
 * I thank Yury Malkov for the ideas and scrutiny of my tests.
 * In particular, for the discussion on how data dependencies and
 * compiler optimizations may affect the outcome.
 *
 * The recommended make command: make -f Makefile.gcc_Ofast testdiv
 */

template <class T>
void testDivMalkovDataDep0(size_t N = 210000000, size_t rep = 1) {
    T c1=1.0,b1=0.9;        
    T c2=1.0,b2=0.91;        
    T c3=1.0,b3=0.92;  
    T c4=1.0,b4=0.93;  

    WallClockTimer timer;

    T coeff = 1/T(rep) * 0.00001;

    for(size_t j = 0; j < N; j++){            
        for(size_t i = 0; i < rep; ++i) {
            c1+=b1/c4;
            c2+=b2/c1;
            c3+=b3/c2;
            c4+=b4/c3;
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
void testDivMalkovDataDep1(size_t N = 210000000, size_t rep = 1) {
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
void testMulMalkovDataDep0(size_t N = 210000000, size_t rep = 1) {
    T c1=1.0,b1=0.9;        
    T c2=1.0,b2=0.91;        
    T c3=1.0,b3=0.92;  
    T c4=1.0,b4=0.93;  

    WallClockTimer timer;

    T coeff = 1/T(rep) * 0.00001;

    for(size_t j = 0; j < N; j++){            
        for(size_t i = 0; i < rep; ++i) {
            c1+=b1*c4;
            c2+=b2*c1;
            c3+=b3*c2;
            c4+=b4*c3;
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
void testMulMalkovDataDep1(size_t N = 210000000, size_t rep = 1) {
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

int main() {
    SetHighAccuracy();

    testDivMalkovDataDep0<float>(100000, 512);
    if (!USE_ONLY_FLOAT) {
      testDivMalkovDataDep0<double>(100000, 512);
      testDivMalkovDataDep0<long double>(100000, 512);
    }

    testDivMalkovDataDep1<float>(100000, 512);
    if (!USE_ONLY_FLOAT) {
      testDivMalkovDataDep1<double>(100000, 512);
      testDivMalkovDataDep1<long double>(100000, 512);
    }

    testMulMalkovDataDep0<float>(100000, 512);
    if (!USE_ONLY_FLOAT) {
      testMulMalkovDataDep0<double>(100000, 512);
      testMulMalkovDataDep0<long double>(100000, 512);
    }

    testMulMalkovDataDep1<float>(100000, 512);
    if (!USE_ONLY_FLOAT) {
      testMulMalkovDataDep1<double>(100000, 512);
      testMulMalkovDataDep1<long double>(100000, 512);
    }
}
