all: regular fast_math 
regular: test_nan_inf.cpp makefile my_isnan_isinf.h
	g++ test_nan_inf.cpp -std=c++11 -O3 -DEXHAUSTIVE_TESTS -o regular
fast_math: test_nan_inf.cpp makefile my_isnan_isinf.h
	g++ test_nan_inf.cpp -std=c++11 -O3 -ffast-math -o fast_math
clean:
	rm regular fast_math
