# This option worsk only for GCC
# GCC_VECT_OPT -ftree-vectorizer-verbose=7
CXXFLAGS=-std=c++11  -march=native -O3 -funroll-loops -ftree-vectorize $(GCC_VECT_OPT)
#CXXFLAGS=-std=c++11  -march=native -Ofast -funroll-loops -ftree-vectorize $(GCC_VECT_OPT)
#CXXFLAGS=-std=c++11  -march=native -Ofast #-funroll-loops -ftree-vectorize $(GCC_VECT_OPT)
membench: membench.cpp makefile
clean: 
	rm -f *.o membench 
membench: membench.o
	$(CXX) -o $@ membench.o -pthread
