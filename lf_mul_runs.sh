#!/bin/bash

make default
mkdir -p output
for z in {1,2,3}
do
    for lf in {0.5,0.75,1,1.25,1.5}
    do
        mkdir -p load_factor_var/${lf}
        for s in {0..9}
        do
            echo workloads/lf_${lf}_zipf_${z}_chained_default_100M
            taskset -c 39 ./benchmark.out workloads/lf_${lf}_zipf_${z}_chained_default_100M ${s}
            echo "--------"
            sleep 10

            echo workloads/lf_${lf}_zipf_${z}_chained_sorted_100M
            taskset -c 39 ./benchmark.out workloads/lf_${lf}_zipf_${z}_chained_sorted_100M ${s}
            echo "--------"
            sleep 10

            cp output/lf_${lf}_zipf_${z}_chained_default_100M load_factor_var/${lf}/lf_${lf}_zipf_${z}_chained_default_100M_${s}
            cp output/lf_${lf}_zipf_${z}_chained_sorted_100M load_factor_var/${lf}/lf_${lf}_zipf_${z}_chained_sorted_100M_${s}
        done
    done
done
