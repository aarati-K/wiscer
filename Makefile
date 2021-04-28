CC=g++ -O3

default: benchmark

workload.o: workload.cpp workload.h
	$(CC) -c workload.cpp

chained.o: chained.cpp chained.h hashmap.h
	$(CC) -c chained.cpp

chained_adaptive.o: chained_adaptive.h chained_adaptive.cpp hashmap.h
	$(CC) -c chained_adaptive.cpp

main.o: main.cpp
	$(CC) -c main.cpp

benchmark: main.o workload.o chained.o chained_adaptive.o
	$(CC) -o benchmark.out main.o workload.o chained.o chained_adaptive.o

clean:
	rm -f *.o
	rm -f *.out
	rm -f benchmark
	rm -f *.gch
