# Where do we find the GNU scientific library?
CXXFLAGS=-Wall -std=c++0x -pedantic -Wold-style-cast -Wall -Wextra -Wcast-align -Wunsafe-loop-optimizations -Wcast-qual -I../.. -O3

test_stringstream: test_stringstream.o
	 $(CXX) -o $@ test_stringstream.o

clean:
	 rm -f *.o test_stringstream
