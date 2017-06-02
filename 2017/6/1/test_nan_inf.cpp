#include <iostream>
#include <cmath>
#include <cfloat>
#include <cstdlib>
#include <limits>

#include "my_isnan_isinf.h"

using namespace std;

void print_stand_isnaninf() {
  cout << boolalpha
            << "===============================================\n" 
            << "isnan(NaN) = " << isnan(NAN) << '\n'
            << "isnan(Inf) = " << isnan(INFINITY) << '\n'
            << "isnan(0.0) = " << isnan(0.0) << '\n'
            << "isnan(Inf - Inf)   = " << isnan(INFINITY - INFINITY) << '\n'
            << "isnan(Inf + Inf)   = " << isnan(INFINITY + INFINITY) << '\n'
            << "isnan(DBL_MIN/2.0) = " << isnan(DBL_MIN/2.0) << '\n'
            << "isnan(0.0 / 0.0)   = " << isnan(0.0/0.0) << '\n'
            << "===============================================\n" 
            << "isinf(NaN) = " << isinf(NAN) << '\n'
            << "isinf(Inf) = " << isinf(INFINITY) << '\n'
            << "isinf(0.0) = " << isinf(0.0) << '\n'
            << "isinf(Inf - Inf)   = " << isinf(INFINITY - INFINITY) << '\n'
            << "isinf(Inf + Inf)   = " << isinf(INFINITY + INFINITY) << '\n'
            << "isinf(DBL_MIN/2.0) = " << isinf(DBL_MIN/2.0) << '\n'
            << "isinf(0.0 / 0.0)   = " << isinf(0.0/0.0) << '\n';
}

void print_my_isnaninf() {
  cout << boolalpha
            << "===============================================\n" 
            << "my_isnan(NaN) = " << my_isnan(NAN) << '\n'
            << "my_isnan(Inf) = " << my_isnan(INFINITY) << '\n'
            << "my_isnan(0.0) = " << my_isnan(0.0) << '\n'
            << "my_isnan(Inf - Inf)   = " << my_isnan(INFINITY - INFINITY) << '\n'
            << "my_isnan(Inf + Inf)   = " << my_isnan(INFINITY + INFINITY) << '\n'
            << "my_isnan(DBL_MIN/2.0) = " << my_isnan(DBL_MIN/2.0) << '\n'
            << "my_isnan(0.0 / 0.0)   = " << my_isnan(0.0/0.0) << '\n'
            << "===============================================\n" 
            << "my_isinf(NaN) = " << my_isinf(NAN) << '\n'
            << "my_isinf(Inf) = " << my_isinf(INFINITY) << '\n'
            << "my_isinf(0.0) = " << my_isinf(0.0) << '\n'
            << "my_isinf(Inf - Inf)   = " << my_isinf(INFINITY - INFINITY) << '\n'
            << "my_isinf(Inf + Inf)   = " << my_isinf(INFINITY + INFINITY) << '\n'
            << "my_isinf(DBL_MIN/2.0) = " << my_isinf(DBL_MIN/2.0) << '\n'
            << "my_isinf(0.0 / 0.0)   = " << my_isinf(0.0/0.0) << '\n';
}

#ifdef EXHAUSTIVE_TESTS
void test_float() {
  union {
    uint32_t u;
    float    f;
  } t;  

  cout << "Exhaustively testing ALL float values" << endl;

  for (size_t i = 0; i <= numeric_limits<uint32_t>::max(); ++i) {
    t.u = static_cast<unsigned>(i);
    if (isnan(t.f) != my_isnan(t.f)) {
      cerr << "Isnan vs my_isnan mismatch for i = " << i << " float number: " << t.f << endl;
      exit(1);
    }
    if (isinf(t.f) != my_isinf(t.f)) {
      cerr << "Isnan vs my_isnan mismatch for i = " << i << " float number: " << t.f << endl;
      exit(1);
    }
  }

  cout << "Floats are fine" << endl;
}
void test_double() {
  union {
    uint32_t u[2];
    double   f;
  } t;  

  cout << "Exhaustively testing a huge range of double values" << endl;
  for (unsigned k = 0; k < 2; ++k)
  for (size_t i = 0; i <= numeric_limits<uint32_t>::max(); ++i) {
    t.u[k] = static_cast<unsigned>(i);
    t.u[1-k] = 0;
    if (isnan(t.f) != my_isnan(t.f)) {
      cerr << "Isnan vs my_isnan mismatch for i = " << i << " double number: " << t.f << endl;
      exit(1);
    }
    if (isinf(t.f) != my_isinf(t.f)) {
      cerr << "Isnan vs my_isnan mismatch for i = " << i << " double number: " << t.f << endl;
      exit(1);
    }
  }
  cout << "Doubles are fine" << endl;
}
#endif
 
int main()
{
  print_stand_isnaninf();
  print_my_isnaninf();
#ifdef EXHAUSTIVE_TESTS
  test_float();
  test_double();
#endif
  return 0;
}
