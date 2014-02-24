/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Leonid Boytsov, http://boytsov.info
 */

#include "cmn.h"

using namespace std;

template <class T>
void testSin(int N, int rep) {
    vector<T>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = 1 + (rand() % 10000) / 1000.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += sin(data[i]); 
            sum += sin(data[i+1]); 
            sum += sin(data[i+2]); 
            sum += sin(data[i+3]); 
        }
        sum /= N*4;
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Sins computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of Sins per sec: " << (float(TotalQty) / t) << endl;
    
}

template <class T>
void testCos(int N, int rep) {
    vector<T>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = 1 + (rand() % 10000) / 1000.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += cos(data[i]); 
            sum += cos(data[i+1]); 
            sum += cos(data[i+2]); 
            sum += cos(data[i+3]); 
        }
        sum /= N*4;
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Cosines computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of Cosines per sec: " << (float(TotalQty) / t) << endl;
    
}

template <class T>
void testAtan(int N, int rep) {
    vector<T>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = 1 + (rand() % 10000) / 1000.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += atan(data[i]); 
            sum += atan(data[i+1]); 
            sum += atan(data[i+2]); 
            sum += atan(data[i+3]); 
        }
        sum /= N*4;
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Atans computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of Atans per sec: " << (float(TotalQty) / t) << endl;
    
}

int main() {
    SetHighAccuracy();
    testSin<float>(10000, 2000);
    testSin<double>(10000, 2000);
    testSin<long double>(10000, 2000);
    testCos<float>(10000, 2000);
    testCos<double>(10000, 2000);
    testCos<long double>(10000, 2000);
    testAtan<float>(10000, 2000);
    testAtan<double>(10000, 2000);
    testAtan<long double>(10000, 2000);
}
