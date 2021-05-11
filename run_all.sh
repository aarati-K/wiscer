#!/bin/bash

make
for filename in workloads/*; do
	echo $filename
	taskset -c 39 ./benchmark.out $filename
	echo "--------"
	sleep 30
done
