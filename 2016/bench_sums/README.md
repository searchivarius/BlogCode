Testing several ways to sum up floats stored in 128-bit SSE and 256-bit AVX registers
====================
Tested using gcc 4.8.4 & Linux Ubuntu 14. CPUS is ``Intel(R) Core(TM) i7-4700MQ CPU @ 2.40GHz
`` Type make and run two binaries. 

Currently, performance numbers are (see code for details) for the 128-bit SSE registers:
```
./testsum128 
Function: PFfU8__vectorfE Ignore: 7.03693e+13Total time: 1858.08 (ms) 
Function: PFfU8__vectorfE Ignore: 7.03693e+13Total time: 2281.51 (ms) 
Function: PFfU8__vectorfE Ignore: 7.03693e+13Total time: 2457.27 (ms) 
```
For 256-bit AVX registers:
```
./testsum256 
Function: PFfU8__vectorfE Ignore: 1.40741e+14Total time: 2393.62 (ms) 
Function: PFfU8__vectorfE Ignore: 1.40741e+14Total time: 2606.65 (ms) 
```
In short, the most naive implementation that relies **on the conversion to the array of scalars** sum works best.
