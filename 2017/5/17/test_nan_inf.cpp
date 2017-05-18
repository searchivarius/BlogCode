#include <iostream>
#include <cmath>
#include <cfloat>
 
int main()
{
    std::cout << std::boolalpha
              << "===============================================\n" 
              << "isnan(NaN) = " << std::isnan(NAN) << '\n'
              << "isnan(Inf) = " << std::isnan(INFINITY) << '\n'
              << "isnan(0.0) = " << std::isnan(0.0) << '\n'
              << "isnan(Inf - Inf)   = " << std::isnan(INFINITY - INFINITY) << '\n'
              << "isnan(Inf + Inf)   = " << std::isnan(INFINITY + INFINITY) << '\n'
              << "isnan(DBL_MIN/2.0) = " << std::isnan(DBL_MIN/2.0) << '\n'
              << "isnan(0.0 / 0.0)   = " << std::isnan(0.0/0.0) << '\n'
              << "===============================================\n" 
              << "isinf(NaN) = " << std::isinf(NAN) << '\n'
              << "isinf(Inf) = " << std::isinf(INFINITY) << '\n'
              << "isinf(0.0) = " << std::isinf(0.0) << '\n'
              << "isinf(Inf - Inf)   = " << std::isinf(INFINITY - INFINITY) << '\n'
              << "isinf(Inf + Inf)   = " << std::isinf(INFINITY + INFINITY) << '\n'
              << "isinf(DBL_MIN/2.0) = " << std::isinf(DBL_MIN/2.0) << '\n'
              << "isinf(0.0 / 0.0)   = " << std::isinf(0.0/0.0) << '\n';
}
