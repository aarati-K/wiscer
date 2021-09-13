#!/bin/bash

make default
mkdir -p output
for z in {1,2,3}
do
    echo workloads/learning_efficiency_roofline_zipf_${z}_default
    taskset -c 39 ./benchmark.out workloads/learning_efficiency_roofline_zipf_${z}_default
    echo "--------"

    echo workloads/learning_efficiency_roofline_zipf_${z}_sorted
    taskset -c 39 ./benchmark.out workloads/learning_efficiency_roofline_zipf_${z}_sorted
    echo "--------"
done
