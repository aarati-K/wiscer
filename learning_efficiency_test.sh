#!/bin/bash

make default
mkdir -p output
mkdir -p learning_efficiency
for r in {0.5,1,2,4,8,16,32,64}
do
    echo hms/hashmap_${r}n.out
    taskset -c 39 ./benchmark.out workloads/learning_efficiency_test_zipf_1 0 hms/hashmap_${r}n.out
    cp output/learning_efficiency_test learning_efficiency/zipf_1_${r}n
    echo "--------"
    sleep 10
done
