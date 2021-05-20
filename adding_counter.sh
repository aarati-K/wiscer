#!/bin/bash

make
for z in {0..5}
do
    mkdir -p adding_counter_zipf${z}/adding_counter
    echo "zipf${z}"
    for i in {1..10}
    do
        taskset -c 39 ./benchmark.out workloads/zipf_${z}_chained_default_100M
        cp output/zipf_${z}_chained_default_100M_adding_counter adding_counter_zipf${z}/adding_counter/${i}
        sleep 10
    done
    echo "--------"
done
