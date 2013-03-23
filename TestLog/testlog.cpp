/**
 * This is code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Leonid Boytsov, http://boytsov.info
 */

#include "ztimer.h"

using namespace std;

template <class T>
void testLog(int N, int rep) {
    vector<T>   data(N);

    for (int i = 0; i < N; ++i) {
        data[i] = 1 + (rand() % 10000) / 1000.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int j = 0; j < rep; ++j) 
    for (int i = 0; i < N; ++i) {
        sum += log(data[i]); 
    }
    timer.split();
    uint64_t t = timer.elapsed();
    cout << "Ignore: " << sum << endl;
    cout << "Logs computed: " << rep * N << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of Logs per sec: " << (uint64_t)(float(rep) * N / (t)) << endl;
    
}

int main() {
    testLog<float>(1e6, 100);
    testLog<double>(1e6, 100);
    testLog<long double>(1e6, 100);
}
