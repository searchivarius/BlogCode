Various tests (in C++) language.

How long does it take to compute a log, a pow, an exponent, etc?
Is division slower than multiplication?
There are 2 makefiles here.


    To build using gcc:

    make -f Makefile.gcc
    
    There is also a makefile for the Intel compiler. To better understand benchmark results,
    please, read my blog entry. This should be important: 
    http://searchivarius.org/blog/problem-previous-version-intels-library-benchmark

    To build with the Intel compiler type:

    make -f Makefile.icc
              

