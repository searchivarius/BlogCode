CXXFLAGS += -std=c++11

# Debug flags
#CXXFLAGS += -g
#CXXFLAGS += -g -O3
CXXFLAGS += -O3

all: bunit

clean: 
	rm -f *.o bunit

self_test.o : *.h *.hpp
bunit.o : *.h *.hpp

bunit: self_test.o bunit.o makefile
	$(CXX) -o$@ self_test.o bunit.o
