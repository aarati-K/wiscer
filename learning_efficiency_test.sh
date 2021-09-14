#!/bin/bash

make default
mkdir -p output
mkdir -p learning_efficiency
for z in {1,2,3}
do
    for r in {0.5,1,2,4,8,16,32,64}
    do
        echo hms/hashmap_zipf_${z}_${r}n.out
        taskset -c 39 ./benchmark.out workloads/learning_efficiency_test_zipf_{z} 0 hms/hashmap_zipf_${z}_${r}n.out
        cp output/learning_efficiency_test learning_efficiency/zipf_${z}_${r}n
        echo "--------"
        sleep 10
    done
done
