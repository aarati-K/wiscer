CC=g++ -O3

all: main.o workload.o metrics.o chained.o chained_adaptive.o
	$(CC) -o benchmark.out main.o workload.o metrics.o chained_adaptive.o chained.o

default: main.o workload.o metrics.o chained.o chained_adaptive_dummy.o
	$(CC) -o benchmark.out main.o workload.o metrics.o chained.o chained_adaptive_dummy.o

adaptive: main.o workload.o metrics.o chained_dummy.o chained_adaptive.o
	$(CC) -o benchmark.out main.o workload.o metrics.o chained_adaptive.o chained_dummy.o

learning: main.o workload.o metrics.o chained_learning.o chained_adaptive_dummy.o
	$(CC) -o benchmark.out main.o workload.o metrics.o chained_learning.o chained_adaptive_dummy.o

workload.o: workload.cpp workload.h
	$(CC) -c workload.cpp

metrics.o: metrics.cpp metrics.h
	$(CC) -c metrics.cpp

chained.o: chained.cpp chained.h hashmap.h
	$(CC) -c chained.cpp

chained_dummy.o: chained_dummy.cpp chained_dummy.h
	$(CC) -c chained_dummy.cpp

chained_learning.o: chained_learning.cpp chained_learning.h
	$(CC) -c chained_learning.cpp

chained_adaptive.o: chained_adaptive.h chained_adaptive.cpp hashmap.h
	$(CC) -c chained_adaptive.cpp

chained_adaptive_dummy.o: chained_adaptive_dummy.cpp chained_adaptive_dummy.h
	$(CC) -c chained_adaptive_dummy.cpp

main.o: main.cpp
	$(CC) -c main.cpp

clean:
	rm -f *.o
	rm -f *.out
	rm -f benchmark
	rm -f *.gch
