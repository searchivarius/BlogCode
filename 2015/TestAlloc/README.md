Measuring the speed of allocation in a somewhat naive way. 
On my Ubuntu 14, Core i7 laptop, one allocation takes approximately 20-200 nanoseconds, or roughly 60-600 CPU cycles. Thus, we can carry out about 0.5-50 million allocations per second. 

Results are consistent with the following benchmark: http://www.helsinki.fi/~joorava/memory/ 
