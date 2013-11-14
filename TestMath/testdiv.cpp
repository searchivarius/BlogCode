/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Leonid Boytsov, http://boytsov.info
 */

#include "cmn.h"

using namespace std;

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
    testDiv<float>(10000, 20000);
    testDiv<double>(10000, 20000);
    testDiv<long double>(10000, 20000);
    testMult<float>(10000, 20000);
    testMult<double>(10000, 20000);
    testMult<long double>(10000, 20000);
}
