/**
 * This is code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Leonid Boytsov, http://boytsov.info
 */

#include <stdexcept>

using namespace std;

#include "ztimer.h"

template <class T>
T PowOptimPosExp(T Base, unsigned Exp);

// Don't use this template for 4-byte integers
template <int>
int PowOptimPosExp(int Base, unsigned Exp) {
    throw runtime_error("Use larger types for Base!");
}

// Don't use this template for low-degree power
template <unsigned int>
int PowOptimPosExp(int Base, unsigned Exp) {
    throw runtime_error("Use larger types for Base!");
}

template <class T>
T PowOptimPosExp(T Base, unsigned Exp) {
    if (!Exp) return 1;
    T res = Base;
    T BaseMin1 = Base - 1;

    --Exp;

    while (Exp) {
        if (Exp & 1) res *= Base;

        Base *= Base;
        Exp >>= 1;

    };

    return res;
};

using namespace std;

template <class T>
void testPow(int N, int rep) {
    vector<T>   data1(N*4);
    vector<T>   data2(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data1[i] = 1 + (rand() % 10000) / 1000.0;
        data2[i] = 1 + (rand() % 10000) / 1000.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += pow(data1[i],   data2[i]); 
            sum += pow(data1[i+1], data2[i+1]); 
            sum += pow(data1[i+2], data2[i+2]); 
            sum += pow(data1[i+3], data2[i+3]); 
        }
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Pows computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of Pows per sec: " << (float(TotalQty) / t) << endl;
    
}

template <class T>
void testIntPowOptim(int IntExp, int N, int rep) {
    vector<T>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = 1 + (rand() % 10000) / 1000.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += PowOptimPosExp(data[i],   IntExp); 
            sum += PowOptimPosExp(data[i+1], IntExp); 
            sum += PowOptimPosExp(data[i+2], IntExp); 
            sum += PowOptimPosExp(data[i+3], IntExp); 
        }
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Pows (optimized) computed, degree: " << IntExp << " TotalQty: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of integer (optimized) Pows per sec: " << (float(TotalQty) / t) << endl;
    
}

template <class T>
void testIntPow(int IntExp, int N, int rep) {
    vector<T>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = 1 + (rand() % 10000) / 1000.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += pow(data[i],   IntExp); 
            sum += pow(data[i+1], IntExp); 
            sum += pow(data[i+2], IntExp); 
            sum += pow(data[i+3], IntExp); 
        }
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Pows computed, degree: " << IntExp << " TotalQty: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of integer Pows per sec: " << (float(TotalQty) / t) << endl;
    
}

int main() {
    testPow<float>(10000, 200);
    testPow<double>(10000, 200);
    testPow<long double>(10000, 200);

    for (int i = 0; i < 10; ++i) {
        testPow<float>(10000, 200);
    }
    for (int i = 0; i < 10; ++i) {
        testIntPow<float>(i, 10000, 200);
    }
    for (int i = 0; i < 10; ++i) {
        testIntPowOptim<float>(i, 10000, 200);
    }
}
