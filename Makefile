
CFLAGS=-g
CXXFLAGS=$(CFLAGS)

genfptest: test.o
	g++ -o genfptest test.o -lgtest

test.o: test.cpp genericfp.hpp

clean:
	rm -f genfptest *.o
