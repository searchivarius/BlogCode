The file [my_isnan_isinf.h](my_isnan_isinf.h) contains functions checking for NaNs and INFs that work even with flags ``-Ofast`` and ``-Offast-math``. See also [a short note here](http://searchivarius.org/blog/gcc-disables-isnan-and-isinf-when-compiling-ffast-math-flag).

There are two versions of the binary built from the same source. The first is ``regular`` and the second is ``fast_math``. The first binary is built without ``-Offast-math`` and without ``-Ofast``. It runs tests where the output of my functions are compared against the output of the standard ``isnan`` and ``isinf``. 

For single-precision numbers, I go over all possible floats (there are only 4B+ of them). For doubles, I can't do this, but the tests include a huge range as well. In particular, I go through all of the possible combinations of the first 34 bits. This should cover all possible corner cases, including signaling NaNs, infinities, and denormalized numbers.

Although I didn't check this, I suspect that the code should work on bigendian machines as well, because the byte layout of integers should be the same as the layout of floating-point numbers of the same length.
