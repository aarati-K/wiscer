#!/bin/bash

make
mkdir adding_counter_zipf0
for i in {1..10}
do
    echo ${i}
    taskset -c 39 ./benchmark.out workloads/zipf_0_chained_default_100M
    echo "--------"
    cp output/zipf_0_chained_default_100M_adding_counter adding_counter_zipf0/${i}
    sleep 10
done
