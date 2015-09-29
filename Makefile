all: maze

maze: maze.o
	g++ -pthread maze.o  -o maze

RatThread.o: RatThread.h maze.cpp
	g++ -pthread -c maze.cpp

clean:
	\rm *.o *~ maze

tar:
	tar cfv proj3.tar maze.cpp RatThread.h Makefile README
