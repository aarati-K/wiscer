#!/bin/bash

# Note: need to change the "chained.h" include to "chained_learning.h"
# in workload.h and chained_adaptive.h
make learning
mkdir -p output
mkdir -p hms
for z in {1,}
do
    for r in {0.0625,0.125,0.25,0.5,1,2,4,8,16,32,64,128}
    do
        echo workloads/learning_efficiency_learn_zipf_${z}_${r}n
        taskset -c 39 ./benchmark.out workloads/learning_efficiency_learn_zipf_${z}_${r}n
        cp hashmap.out hms/hashmap_zipf_${z}_${r}n.out
        echo "--------"
    done
done
