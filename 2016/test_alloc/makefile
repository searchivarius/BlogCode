CXXFLAGS=-std=c++11 -DHAVE_CXX0X -openmp   -march=native -Ofast
#CXXFLAGS=-std=c++11 -DHAVE_CXX0X -openmp -march=native  -g
all: run
clean: 
	rm -f *.o run
run: run.o
	$(CXX) -o $@ run.o
