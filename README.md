Various tests (in C++) language.

1. TestMath -- How long does it take to compute a log, a pow, an exponent, etc?
   Is division slower than multiplication?
   There are 2 makefiles in TestMath. One is to be used with GNU C++, another with the Intel C++ compiler.

    To build with g++ type:

    make -f Makefile.gcc

    To build with the Intel compiler type (read my blog entry to better understand some results http://searchivarius.org/blog/problem-previous-version-intels-library-benchmark):

    make -f Makefile.icc
              
2.  BenchCPUCores -- a stupid code to get the number of cores empirically.  Compile the code (type make) and execute ./bench.sh with a large parameter N. This script runs the compiled binary using different number of threads, each of which carries out the same task. While the number of threads does not exceed the number of actual cores, the run-time remains constant. When, you see the increase in the run-time, the number of threads exceeds the number of actual cores. The parameter N should be chosen large enough. For instance:

    ./bench.sh 1024 
