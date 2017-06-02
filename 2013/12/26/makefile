BINS=testapproxlog 

CXXFLAGS+=-I..  -static -Ofast -march=native -std=c++11 -D__extern_always_inline=inline

all: $(BINS)

testapproxlog: testapproxlog.o
	$(CXX) testapproxlog.o -o testapproxlog -static

clean:
	rm -f $(BINS) *.o
