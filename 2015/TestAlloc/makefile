BINS=test_alloc

CXXFLAGS+= -static -Ofast -std=c++11 -DHAVE_CXX0X  -march=native

all: $(BINS)

test_alloc: test_alloc.o
	$(CXX) test_alloc.o -o test_alloc -static

clean:
	rm -f $(BINS) *.o
