#include <cstdint>
#include <iostream>
#include <sys/time.h>

using namespace std;

/**
 *  author: Preston Bannister
 */
class WallClockTimerBannister {
public:
    struct timeval t1, t2;
    WallClockTimerBannister() :
        t1(), t2() {
        gettimeofday(&t1, 0);
        t2 = t1;
    }
    void reset() {
        gettimeofday(&t1, 0);
        t2 = t1;
    }
    uint64_t elapsed() {
        return ((t2.tv_sec - t1.tv_sec) * 1000ULL * 1000ULL) + ((t2.tv_usec - t1. tv_usec));
    }
    uint64_t split() {
        gettimeofday(&t2, 0);
        return elapsed();
    }
};

void BurnCPU(size_t qty = 10000000000) {
  size_t sum = 0;
  for (size_t i = 0; i < qty; ++i) {
    sum += i;
    sum *= qty;
  }
  cout << "Ignore: " << sum << endl;
}

#include "ztimer.h"

int main() {
  WallClockTimerBannister oldz;
  WallClockTimer          z;

  BurnCPU();
  oldz.split();
  z.split();

  cout << "Timer, old vs new : " << z.elapsed() << " : " << oldz.elapsed() << endl;
  // We expect both timers to differ in at most 1 ms

  BurnCPU();
  oldz.split();
  z.split();

  cout << "Timer, old vs new : " << z.elapsed() << " : " << oldz.elapsed() << endl;
  // We expect both timers to differ in at most 1 ms

  z.reset();
  oldz.reset();

  BurnCPU();
  oldz.split();
  z.split();

  cout << "Timer, old vs new : " << z.elapsed() << " : " << oldz.elapsed() << endl;
  // We expect both timers to differ in at most 1 ms
}


