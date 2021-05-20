#!/bin/bash

make
mkdir adding_counter_zipf1
for i in {1..10}
do
    echo ${i}
    taskset -c 39 ./benchmark.out workloads/zipf_1_chained_default_100M
    echo "--------"
    cp output/zipf_1_chained_default_100M_adding_counter adding_counter_zipf1/${i}
    sleep 10
done
