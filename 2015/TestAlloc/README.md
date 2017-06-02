Measuring the speed of allocation in a somewhat naive way. You need several gigabytes of memory (I had 16) to use this (or else update the code).

For **small and medium objects**, on my Ubuntu 14, Core i7 16GB laptop, one allocation takes approximately 20-200 nanoseconds, or roughly 60-600 CPU cycles. Thus, we can carry out about 0.5-50 million allocations per second.  Results are consistent with the following benchmark: http://www.helsinki.fi/~joorava/memory/ 

However, if the system is forced to zero-out **huge** pages (we simulate this by carrying out **memset** ourselves), the cost per allocation can be much larger (even in the order of 1 ms). This observation is consistent with this blog post: https://randomascii.wordpress.com/2014/12/10/hidden-costs-of-memory-allocation/

A good question is how frequently the OS needs to zero-out content. If the page is going to be returned to the same process, this is necessary. However, if the page is returned to the common memory pool (does this happen frequently??) memory needs to be zeroed-out.
