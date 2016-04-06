CXXFLAGS += -std=c++11

# Debug flags
#CXXFLAGS += -g
#CXXFLAGS += -g -O3
CXXFLAGS += -O3

all: bunit perf_benchmark 

clean: 
	rm -f *.o bunit perf_benchmark

self_test.o : *.h *.hpp
bunit.o : *.h *.hpp
perf_benchmark.o : *.h *.hpp tachymeter/*.hpp

bunit: self_test.o bunit.o makefile
	$(CXX) -o$@ self_test.o bunit.o

perf_benchmark: perf_benchmark.o  makefile
	$(CXX) -o$@ perf_benchmark.o
