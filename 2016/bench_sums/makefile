CXXFLAGS=-std=c++11 -DHAVE_CXX0X -openmp   -march=native
all: testsum128 testsum256
clean: 
	rm -f *.o testsum128 testsum256
testsum128: testsum128.o
	$(CXX) -o $@ testsum128.o
testsum256: testsum256.o
	$(CXX) -o $@ testsum256.o
