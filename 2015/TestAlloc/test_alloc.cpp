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

void test(size_t allocQty, const size_t minAllocSize, const size_t maxAllocSize, unsigned repQty, bool bNonZero) {
  vector<void*> addr(2*allocQty);

  size_t start1 = 0, start2 = allocQty;

  WallClockTimer timer;

  size_t sum = 0, totOpQty;

  for (unsigned i = 0; i < repQty; ++i) { 
    for (unsigned j = 0; j < allocQty; ++j) {
      size_t randSize = max<size_t>(minAllocSize + rand() % (maxAllocSize - minAllocSize),1); 
      addr[start1 + j] = malloc(randSize);
      if (bNonZero) {
        memset(addr[start1 + j], 255, randSize);
      }
      size_t randBytePos = rand() % randSize;
      char   randByte = (char) rand() & 255;
      ((char*)addr[start1 + j])[randBytePos] = randByte; // Modify a byte so that the page isn't all zeros any more
      // Above it's ensured randSize >= sizeof(size_t)
      sum += ((char*)addr[start1 + j])[randBytePos];
      free(addr[start2 + j]);
      totOpQty += 2;
    }

    swap(start1, start2);
  }

  uint64_t   total = timer.split();

  cout << "Ignore: " << sum << endl;
  cout << "Memsetting allocating memory: " << bNonZero << endl; 
  cout << "The # of alloc/free operations: " << totOpQty << " Elapsed: " << total / 1e3 << " ms "; 
  cout << "Each operation took: " << total / 1e3 / float(totOpQty) << " ms" << endl;
}; 

int main(int,char**) {
  for (int flag = 0; flag < 2; ++flag) {
    cout << "Small object tests";
    test(1000*1024, 1, 32, 10, flag != 0);
    test(1000*1024, 1, 1*1024, 10, flag != 0);
    test(1000*1024, 1, 4*1024, 10, flag != 0);

    cout << "Large object tests";
    test(1000, 1, 32*1024, 20, flag != 0);
    test(1000, 1, 1*1024*1024, 20, flag != 0);
    test(1000, 1, 4*1024*1024, 20, flag != 0);

    cout << "Very large object tests";
    test(100, 1, 320*1024, 10000, flag != 0);
    test(100, 1, 10*1024*1024, 100, flag != 0);
    test(100, 1, 40*1024*1024, 100, flag != 0);
  }

  return 0;
};
