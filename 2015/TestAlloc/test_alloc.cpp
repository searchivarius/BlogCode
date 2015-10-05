/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Leonid Boytsov, http://boytsov.info
 */

#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>

#include "../../2014/ztimer.h"

using namespace std;

void test(size_t allocQty, const size_t minAllocSize, const size_t maxAllocSize, unsigned repQty) {
  vector<void*> addr(2*allocQty);

  size_t start1 = 0, start2 = allocQty;

  WallClockTimer timer;

  size_t sum = 0, totOpQty;

  for (unsigned i = 0; i < repQty; ++i) { 

    for (unsigned j = 0; j < allocQty; ++j) {
      size_t randSize = minAllocSize + rand() % (maxAllocSize - minAllocSize); 
      addr[start1 + j] = malloc(randSize);
      sum += (size_t) addr[start1 + j];
      free(addr[start2 + j]);
      totOpQty += 2;
    }

    swap(start1, start2);
  }

  uint64_t   total = timer.split();

  cout << "Ignore: " << sum << endl;
  cout << "The # of alloc/free operations: " << totOpQty << " Elapsed: " << total / 1e3 << " ms "; 
  cout << "Each operation took: " << total / 1e3 / float(totOpQty) << " ms" << endl;
}; 

int main(int,char**) {
  test(1000, 1, 32*1024, 10000);
  test(1000, 1, 1*1024*1024, 10000);
  test(1000, 1, 4*1024*1024, 10000);
  return 0;
};
