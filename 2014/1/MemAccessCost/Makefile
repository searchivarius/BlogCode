# Where do we find the GNU scientific library?
CXXFLAGS=-Wall -std=c++0x -pedantic -Wold-style-cast -Wall -Wextra -Wcast-align -Wunsafe-loop-optimizations -Wcast-qual -I../.. -O3

test_mem_latency: test_mem_latency.o
	 $(CXX) -o $@ test_mem_latency.o

clean:
	 rm -f *.o test_mem_latency
