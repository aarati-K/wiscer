#!/bin/bash

make
for s in {0..9}
do
    for i in {1..10}
    do
        for z in {0,0.5,1,1.5,2,2.5,3,3.5,4,4.5,5}
        do
            echo workloads/zipf_${z}_chained_default_100M
            taskset -c 39 ./benchmark.out workloads/zipf_${z}_chained_default_100M ${s}
            echo "--------"
            sleep 10

            echo workloads/zipf_${z}_chained_sorted_100M
            taskset -c 39 ./benchmark.out workloads/zipf_${z}_chained_sorted_100M ${s}
            echo "--------"
            sleep 10
        done
        mkdir -p runs/${i}
        cp output/*100M runs/${i}
    done
    cp -r runs runs${s}
done
