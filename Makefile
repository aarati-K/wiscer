CC=g++ -O3

default: benchmark

workload.o: workload.cpp workload.h
	$(CC) -c workload.cpp

chained.o: chained.cpp chained.h hashmap.h
	$(CC) -c chained.cpp chained.h

chained_adaptive.o: chained_adaptive.cpp chained_adaptive.h hashmap.h
	$(CC) -c chained_adaptive.cpp chained_adaptive.h

benchmark: main.cpp workload.o chained.o chained_adaptive.o
	$(CC) -o benchmark.out main.cpp workload.o chained.o chained_adaptive.o

clean:
	rm -f *.o
	rm -f *.out
	rm -f benchmark
	rm -f *.gch
