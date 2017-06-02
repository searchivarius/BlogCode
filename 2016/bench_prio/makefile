CXXFLAGS += -std=c++11

# Debug flags
#CXXFLAGS += -g
#CXXFLAGS += -g -O3
CXXFLAGS += -O3

all: bunit perf_benchmark 

clean: 
	rm -f *.o bunit perf_benchmark

gheap_test.o : bunit.h galgorithm.hpp gheap_cpp11.hpp gheap.hpp gpriority_queue.hpp
prio_test.o : bunit.h prio_queue.hpp
falconn_test.o : bunit.h falconn_heap_mod.h
bunit.o : bunit.h  
perf_benchmark.o : *.h *.hpp tachymeter/*.hpp

bunit: prio_test.o falconn_test.o gheap_test.o bunit.o makefile
	$(CXX) -o$@ prio_test.o falconn_test.o gheap_test.o bunit.o

perf_benchmark: perf_benchmark.o  makefile
	$(CXX) -o$@ perf_benchmark.o
