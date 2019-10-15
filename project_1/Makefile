
quash: quash.o
	g++ -std=c++17 quash.o -o quash

quash.o: quash.cpp quash.hpp jobs.hpp
	g++ -std=c++17 -c quash.cpp

clean:
	rm -f quash *.o 
