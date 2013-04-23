/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Leonid Boytsov, http://boytsov.info
 */

#include "cmn.h"

using namespace std;

template <class T>
void testLog(int N, int rep) {
    vector<T>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = 1 + (rand() % 10000) / 1000.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += log(data[i]); 
            sum += log(data[i+1]); 
            sum += log(data[i+2]); 
            sum += log(data[i+3]); 
        }
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Logs computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of Logs per sec: " << (float(TotalQty) / t) << endl;
    
}

int main() {
    SetHighAccuracy();
    testLog<float>(10000, 2000);
    testLog<double>(10000, 2000);
    testLog<long double>(10000, 2000);
}
