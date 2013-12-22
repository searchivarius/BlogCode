SlowRegularExpressions
======================
This is to illustrate the disadvantage of the backtracing approache (with partial memoization) used in Perl, Java, and many other languages. Written for the blog post: http://searchivarius.org/blog/are-regular-expressions-fast

Just run test.sh. It tries to match a short pattern against two files. In one case, it succeeds and finishes quickly. In another case, it takes 30 seconds (on my old CoreDuo laptop) to figure out that there is no match.


