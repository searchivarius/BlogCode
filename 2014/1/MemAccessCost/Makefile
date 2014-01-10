# Where do we find the GNU scientific library?
CXXFLAGS=-Wall -O3 -std=c++0x -pedantic -Wold-style-cast -Wall -Wextra -Wcast-align -Wunsafe-loop-optimizations -Wcast-qual -I../..

test_mem_latency: test_mem_latency.o
	 $(CXX) -o $@ test_mem_latency.o

clean:
	 rm -f *.o test_mem_latency
