
genfptest: test.o genericfp.o
	g++ -o genfptest test.o genericfp.o -lgtest

genericfp.o: genericfp.cpp genericfp.hpp

test.o: test.cpp genericfp.hpp

clean:
	rm -f genfptest *.o
