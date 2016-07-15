CXXFLAGS=-std=c++11 -DHAVE_CXX0X -openmp   -march=native -Ofast
#CXXFLAGS=-std=c++11 -DHAVE_CXX0X -openmp -march=native  -g
all: bench_dist
clean: 
	rm -f *.o bench_dist
bench_dist: bench_dist.o
	$(CXX) -o $@ bench_dist.o
