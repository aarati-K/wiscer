#!/bin/bash

make
mkdir adding_counter
for i in {1..10}
do
    echo ${i}
    taskset -c 39 ./benchmark.out workloads/zipf_2_chained_default_100M
    echo "--------"
    cp output/*adding_counter adding_counter/${i}
    sleep 10
done
