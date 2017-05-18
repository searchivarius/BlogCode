all: regular fast_math 
regular: test_nan_inf.cpp
	g++ test_nan_inf.cpp -std=c++11  -o regular
fast_math: test_nan_inf.cpp
	g++ test_nan_inf.cpp -std=c++11  -ffast-math -o fast_math
clean:
	rm regular fast_math
