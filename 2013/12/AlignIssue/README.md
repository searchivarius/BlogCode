Show how vectorization may cause a segfault.

Tested on Intel core-i7 with g++-4.7

To create all binaries and assembly code:
make

vectorized_manually_fixed.s is manually fixed assembly code,
where aligned reads vmovapd were replaced with unaligned reads vmovupd.

To compile vectorized_manually_fixed.s, type:

gcc vectorized_manually_fixed.s -lstdc++ -o unaligned_test_manually_fixed

See, the blog entry for details:
