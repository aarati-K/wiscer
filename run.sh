#!/bin/bash

make
for filename in workloads/*; do
	echo $filename
	taskset -c 20 ./benchmark.out $filename
	echo "--------"
	sleep 10
done
