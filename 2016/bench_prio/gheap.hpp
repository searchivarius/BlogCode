// Pass -DGHEAP_CPP11 to compiler for including gheap optimized for C++11.
// Otherwise gheap optimized for C++03 will be included.
// We are not gonna use it with pre C++11 compilers
#define GHEAP_CPP11
#define NDEBUG

#ifdef GHEAP_CPP11
#  include "gheap_cpp11.hpp"
#else
#  include "gheap_cpp03.hpp"
#endif
