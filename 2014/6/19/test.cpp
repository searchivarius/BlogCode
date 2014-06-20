#include "floatdiff.h"
#include <iostream>
#include <limits>

using namespace std;

template <class dist_t>
void test(dist_t x, dist_t y,  size_t maxUlps) {
 
    cout << "===================================" << endl;
    std::cout << x;
    if (FloatingPointDiff<dist_t>(x).AlmostEquals(FloatingPointDiff<dist_t>(y), maxUlps)) {
        cout <<  " is ";
    } else {
        cout << " is not ";
    }
    cout << "nearly equal to " << y << " (considering max diff=" << maxUlps << ")" << endl;

    //
    // Not to be used with NANs, BTW AlmostEquals always returns false
    // if any of the two numbers is NAN !!!!
    //
    dist_t diff = FloatingPointDiff<dist_t>(x).ULP_diff(FloatingPointDiff<dist_t>(y));
    cout << "Difference is : " <<  diff << " ulps " << endl;
    cout << "===================================" << endl;
} 
 
int main()
{
    { // Testing float
      float x = 3.14159625f;

      test(x, x * (1 + 2 * numeric_limits<float>::epsilon()), 4);
      test(x, x * (1 + 3 * numeric_limits<float>::epsilon()), 4);
      test(x, x * (1 + 4 * numeric_limits<float>::epsilon()), 6);
      test(x, x * (1 + 5 * numeric_limits<float>::epsilon()), 6);
    }

    cout << endl << "**********************************" << endl << endl;

    { // Testing double
      double x = 3.14159625;

      test(x, x * (1 + 2 * numeric_limits<double>::epsilon()), 4);
      test(x, x * (1 + 3 * numeric_limits<double>::epsilon()), 4);
      test(x, x * (1 + 4 * numeric_limits<double>::epsilon()), 6);
      test(x, x * (1 + 5 * numeric_limits<double>::epsilon()), 6);
    }

    cout << endl << "**********************************" << endl << endl;

    { // Demonstrate that NANs are never considered to equal no matter how large the threshold is!
      float x = 0.0/0.0;

      test(x, x, 1024 /* large threshold, yet NAN numbers are not considered to be equal! */ );
    }
 
 
    return 0;
}  
