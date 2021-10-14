CC=g++ -O3 -march=native
DEFAULT=-DDEFAULT
VIP=-DVIP
STORE=-DSTORE

default: main.cpp workload.cpp workload.h metrics.cpp metrics.h chained.cpp chained.h storage_engine.cpp
	$(CC) $(DEFAULT) -o benchmark.out main.cpp

vip: main.cpp workload.cpp workload.h metrics.cpp metrics.h chained_adaptive.cpp chained_adaptive.h storage_engine.cpp
	$(CC) $(VIP) -o benchmark.out main.cpp

store: main.cpp workload.cpp workload.h metrics.cpp metrics.h store_workload.cpp store_workload.h storage_engine.cpp
	$(CC) $(STORE) -o benchmark.out main.cpp

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.gch
