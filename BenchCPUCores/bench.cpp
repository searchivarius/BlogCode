#include <iostream>
#include <cmath>
using namespace std;
 
#include <omp.h>
 
int main(int argc, char *argv[])
{
  int sum[1024];
  fill(sum, sum + 1024, 0);
   
  int nthreads, th_id = 0;
  #pragma omp parallel private(th_id)
  { 
    th_id = omp_get_thread_num();
    nthreads = omp_get_num_threads();
    for (int j = 0; j < 1e8; ++j) {
        sum[th_id] += exp(1 + j * 1e-8);
    }
  }

  cout << "Thread qty: " << nthreads << endl;

  for (int i = 0; i < nthreads; ++i)
    cout << "Sum[" << i << "]: " << sum[i] << endl;
 
  return 0;
}
