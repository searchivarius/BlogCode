A stupid utility to estimate the number of cores empirically.  Compile the code (type make) and execute ./bench.sh with a large parameter N. This script runs the compiled binary using different number of threads, each of which carries out the same task. While the number of threads does not exceed the number of actual cores, the run-time remains constant. When, you see the increase in the run-time, the number of threads exceeds the number of actual cores. The parameter N should be chosen large enough. For instance:

    ./bench.sh 1024 
