#!/bin/bash

make
for z in {0,0.5,1,1.5,2,2.5,3,3.5,4}
do
    echo workloads/zipf_${z}_chained_default_1B
    taskset -c 20 ./benchmark.out workloads/zipf_${z}_chained_default_1B
    echo "--------"
    sleep 10

    echo workloads/zipf_${z}_chained_sorted_1B
    taskset -c 20 ./benchmark.out workloads/zipf_${z}_chained_sorted_1B
    echo "--------"
done
