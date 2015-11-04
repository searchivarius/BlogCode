/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Leonid Boytsov, http://boytsov.info
 */

#include <sstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>

#include "../../../2014/ztimer.h"

using namespace std;

void test1(int N, int rep) {
  WallClockTimer timer;

  uint64_t total = 0;

  uint64_t sum = 0;

  for (int j = 0; j < rep; ++j) {

    timer.reset();


    for (int i = 0; i < N; i++) {
      stringstream str;
      sum += reinterpret_cast<size_t>(str.str().c_str());
    }

    total += timer.split();
  }

  cout << "Ignore: " << sum << endl;
  cout << " total # of construct/destruct: " << rep * N << ", time " <<  total / 1e3 << " ms" << " Construct/destruct/ per sec: " << (rep * N * 1e6 / total ) << endl;
}

void test2(int N, int rep) {
  WallClockTimer timer;

  uint64_t total = 0;

  uint64_t sum = 0;

  for (int j = 0; j < rep; ++j) {

    timer.reset();


    for (int i = 0; i < N; i++) {
      stringstream str;
      str << i << " " << j; 
      sum += reinterpret_cast<size_t>(str.str().c_str());
    }

    total += timer.split();
  }

  cout << "Ignore: " << sum << endl;
  cout << " total # of construct/destruct/proc: " << rep * N << ", time " <<  total / 1e3 << " ms" << " Construct/destruct/proc per sec: " << (rep * N * 1e6 / total ) << endl;
}

void test3(int N, int rep) {
  WallClockTimer timer;

  uint64_t total = 0;

  uint64_t sum = 0;

  string       emptyStr;
  stringstream str;

  for (int j = 0; j < rep; ++j) {

    timer.reset();

    for (int i = 0; i < N; i++) {
      str.str(emptyStr);
      str << i << " " << j; 
      sum += reinterpret_cast<size_t>(str.str().c_str());
    }

    total += timer.split();
  }

  cout << "Ignore: " << sum << endl;
  cout << " total # of proc without construct/deconstruct: " << rep * N << ", time " <<  total / 1e3 << " ms" << " proc per sec: " << (rep * N * 1e6 / total ) << endl;
}

int main() {
  test3(1024*1024, 10);
  test1(1024*1024, 10);
  test2(1024*1024, 10);
}
