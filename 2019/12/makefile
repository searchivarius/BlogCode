# This option worsk only for GCC
OPT_FLAG=-Ofast
# GCC_VECT_OPT -ftree-vectorizer-verbose=7
CXXFLAGS=-std=c++11  -march=native $(OPT_FLAG) -funroll-loops -ftree-vectorize $(GCC_VECT_OPT)
membench: membench.o
	$(CXX) -o $@ membench.o -pthread
membench.o: membench.cpp makefile
clean: 
	rm -f *.o membench 
