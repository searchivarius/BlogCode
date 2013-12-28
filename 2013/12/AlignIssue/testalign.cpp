#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

double dist(const double*x,const double*y,size_t qty){
  double res=0;
  for (size_t i = 0; i < qty; i++) 
    res += x[i] - y[i];
  return res;
}


int main(int,char**) {
  int N = 128;
  char *arr1 = new char[sizeof(double)*N*2 + 4 + 1024*N /* a lot to be safe */];
  char *arr2 = new char[sizeof(double)*N *2+ 4 + 1024*N /* a lot to be safe */];
  cout << "Ignore1: " << dist((double*)(arr1),(double*)(arr2), N)<<endl;;
  cout << "Ignore2: " << dist((double*)(arr1 + 4),(double*)(arr2+4), N)<<endl;;
  return 0;
};
