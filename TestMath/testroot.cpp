/**
 * This is code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Leonid Boytsov, http://boytsov.info
 */

#include "ztimer.h"
#include "mkl.h"


using namespace std;

template <class T>
void testExp(int N, int rep) {
    vector<T>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = 1 + (rand() % 10000) / 1000.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += exp(data[i]); 
            sum += exp(data[i+1]); 
            sum += exp(data[i+2]); 
            sum += exp(data[i+3]); 
        }
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = uint64_t(rep) * N * 4LL;
    cout << "Ignore: " << sum << endl;
    cout << "Exps computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of Exps per sec: " << (float(TotalQty) / t) << endl;
    
}

int main() {
    //vmlSetMode(VML_HA);
    vmlSetMode(VML_LA);
    testExp<float>(10000, 2000);
    testExp<double>(10000, 2000);
    testExp<long double>(10000, 2000);
}
