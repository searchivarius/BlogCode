/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Leonid Boytsov, http://boytsov.info
 */

#include "cmn.h"

using namespace std;

template <class T>
void testDivMalkovDataDep(size_t N = 210000000, size_t rep = 1) {
// This test was proposed by Yury Malkov and modified by Leonid Boytsov
    T c1=1.0,b1=0.9;        
    T c2=1.0,b2=0.91;        
    T c3=1.0,b3=0.92;  
    T c4=1.0,b4=0.93;  

    WallClockTimer timer;

    for(size_t j = 0; j < N; j++){            
        c1 = c2 = c3 = c4 = 1.0;
        for(size_t i = 0; i < rep; ++i) {
            c1+=b1/c1;
            c2+=b2/c2;
            c3+=b3/c3;
            c4+=b4/c4;
        }
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    T sum = c1 + c2 + c3 + c4;
    cout << "Ignore: " << sum << endl;
    cout << "Test WITH data dependencies" << endl;
    cout << "DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of DIVs per sec: " << (float(TotalQty) / t) << endl;
}

template <class T>
void testMulMalkovDataDep(size_t N = 210000000, size_t rep = 1) {
// This test was proposed by Yury Malkov and modified by Leonid Boytsov
    T c1=1.0,b1=0.9;        
    T c2=1.0,b2=0.91;        
    T c3=1.0,b3=0.92;  
    T c4=1.0,b4=0.93;  

    WallClockTimer timer;

    for(size_t i = 0; i < rep; ++i) {
        c1 = c2 = c3 = c4 = 1.0;
        for(size_t j = 0; j < N; j++){            
            c1+=b1*c1;
            c2+=b2*c2;
            c3+=b3*c3;
            c4+=b4*c4;
        }
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    T sum = c1 + c2 + c3 + c4;
    cout << "Ignore: " << sum << endl;
    cout << "Test WITH data dependencies" << endl;
    cout << "MULs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of MULs per sec: " << (float(TotalQty) / t) << endl;
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
            sum+=b2/c2;
            sum+=b3/c3;
            sum+=b4/c4;
        }
        sum /= N;
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Test withOUT data dependencies" << endl;
    cout << "Ignore: " << sum << endl;
    cout << "DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of DIVs per sec: " << (float(TotalQty) / t) << endl;
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
            sum+=b2*c2;
            sum+=b3*c3;
            sum+=b4*c4;
        }
        sum /= N;
    }

    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Test withOUT data dependencies" << endl;
    cout << "Ignore: " << sum << endl;
    cout << "MULs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of MULs per sec: " << (float(TotalQty) / t) << endl;
}

template <class T>
void testDiv(int N, int rep) {
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
    cout << "Ignore: " << sum << endl;
    cout << "DIVs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of DIVs per sec: " << (float(TotalQty) / t) << endl;
    
}

template <class T>
void testMult(int N, int rep) {
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
    cout << "Ignore: " << sum << endl;
    cout << "MULs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of MULs per sec: " << (float(TotalQty) / t) << endl;
    
}

int main() {
    SetHighAccuracy();
    testDivMalkovDataDep<float>(10000, 200);
    testDivMalkovDataDep<double>(10000, 200);
    testDivMalkovDataDep<long double>(10000, 200);
    testMulMalkovDataDep<float>(10000, 200);
    testMulMalkovDataDep<double>(10000, 200);
    testMulMalkovDataDep<long double>(10000, 200);
    testDivMalkovNoDataDep<float>(10000, 200);
    testDivMalkovNoDataDep<double>(10000, 200);
    testDivMalkovNoDataDep<long double>(10000, 200);
    testMulMalkovNoDataDep<float>(10000, 200);
    testMulMalkovNoDataDep<double>(10000, 200);
    testMulMalkovNoDataDep<long double>(10000, 200);
    testDiv<float>(10000, 20000);
    testDiv<double>(10000, 20000);
    testDiv<long double>(10000, 20000);
    testMult<float>(10000, 20000);
    testMult<double>(10000, 20000);
    testMult<long double>(10000, 20000);
}
