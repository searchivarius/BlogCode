CXXFLAGS=-Wall  -std=c++0x -pedantic -Wold-style-cast -Wall -Wextra -Wcast-align -Wunsafe-loop-optimizations -Wcast-qual -I../.. -O3

test_disk: test_disk.o
	 $(CXX) -o $@ test_disk.o

clean:
	 rm -f *.o test_disk
