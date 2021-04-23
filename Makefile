CC=g++ -O3

default: benchmark

workload.o: workload.cpp workload.h
	$(CC) -c workload.cpp

chained.o: chained.cpp chained.h hashmap.h
	$(CC) -c chained.cpp chained.h

benchmark: main.cpp workload.o chained.o
	$(CC) -o benchmark.out main.cpp workload.o chained.o

clean:
	rm -f *.o
	rm -f *.out
	rm -f benchmark
	rm -f *.gch
