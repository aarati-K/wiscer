#!/bin/bash

# Note: need to change the "chained.h" include to "chained_learning.h"
# in workload.h and chained_adaptive.h
make learning
mkdir -p output
for r in {0.5,1,2,4,8,16,32,64}
do
    echo workloads/learning_efficiency_learn_zipf_1_${r}n
    taskset -c 39 ./benchmark.out workloads/learning_efficiency_learn_zipf_1_${r}n
    cp hashmap.out hashmap_${r}n.out
    echo "--------"
done
