Testing several ways to sum up floats stored in 128-bit SSE and 256-bit AVX registers
====================
Tested using gcc 4.8.4 & Linux Ubuntu 14. CPUS is ``Intel(R) Core(TM) i7-4700MQ CPU @ 2.40GHz
`` Type make and run two binaries. See also [my blog post for a short explanation](http://searchivarius.org/blog/summing-values-inside-128-bit-sse-or-256-bit-avx-register-its-not-easy-vectorize). Note that some other compilers will not accept C-style casting of registers, you may need to use the following [cast pseudo-intrinsics](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm&techs=SSE,SSE2,SSE3,SSSE3,SSE4_1,SSE4_2,AVX,AVX2&expand=4584,2256,584&cats=Cast).

Currently, performance numbers are (see code for details) for the 128-bit SSE registers:
```
./testsum128 
Function: scalar                	Total time: 302.783 (ms)  Ignore: 7.03693e+13
Function: vector via _mm_hadd_ps	Total time: 469.195 (ms)  Ignore: 7.03693e+13
Function: vector via shuffles1  	Total time: 440.41 (ms)  Ignore: 7.03693e+13
Function: vector via shuffles2  	Total time: 407.912 (ms)  Ignore: 7.03693e+13
```
For 256-bit AVX registers:
```
./testsum256 
Function: scalar          	Total time: 617.095 (ms)  Ignore: 1.40741e+14
Function: part. vectorized	Total time: 939.864 (ms)  Ignore: 1.40741e+14
```
In short, the most naive implementation that relies **on the conversion to the array of scalars** sum works best.
