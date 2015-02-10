BINS=teststringstream

CXXFLAGS+= -static -Ofast -std=c++11 -DHAVE_CXX0X  -march=native

all: $(BINS)

testpow: teststringstream.o
	$(CXX) teststringstream.o -o teststringstream -static

clean:
	rm -f $(BINS) *.o
