#include <iostream>
#include <sstream>
#include <cstdlib>

#include <string.h>
#include <sys/mman.h>

#include "ztimer.h"

using namespace std;

void Test(size_t TotalQty) {


  WallClockTimer wtm;
  CPUTimer       ctm;

  size_t sum = 0;

  wtm.reset();
  ctm.reset();

  for (int i = 0; i < TotalQty; ++i) {
    stringstream* str = new stringstream();    
    sum += (size_t)str;
    delete str;
  }

  wtm.split();
  ctm.split();

  cout << "Wall elapsed: " << wtm.elapsed() / 1e6 << endl;
  cout << "CPU elapsed: " << ctm.elapsed() / 1e6 << endl;
  cout << "Ops per sec: " << (1e6* TotalQty /  wtm.elapsed())  << endl;
  cout << "Ignore: " << sum << " Qty: " << TotalQty  << endl;
  cout << "================================================================================" << endl;
}

int main(int argc, char *argv[]) {
  const size_t Qty = 16UL * 1024UL * 1024UL;

  Test(Qty);

  return 0;

}
