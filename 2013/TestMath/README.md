Various tests (in C++) language.

How long does it take to compute a log, a pow, an exponent, etc?
Is division slower than multiplication?
There are 3 makefiles in TestMath. Two should to be used with GNU C++ (they only differ in the optimization flag), another with the Intel C++ compiler.

    To build with g++ on x86 platform type:

    make -f Makefile.gcc_x86

    To build with g++ on platform other than x86 type:

    make -f Makefile.gcc_other
    
    There is also a makefile for the Intel compiler. To better understand benchmark results,
    please, read my blog entry. This should be important: 
    http://searchivarius.org/blog/problem-previous-version-intels-library-benchmark

    To build with the Intel compiler type:

    make -f Makefile.icc
              

