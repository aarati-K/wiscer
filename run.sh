#!/bin/bash

if [ "$#" -eq 0 ] || ! [ -d "$1" ]; then
  echo "Usage: $0 workload_file {random_seed}"
  exit 1
fi

make default

if grep -q VIPHashing $1; then
    make vip
fi

if grep -q null $1; then
    make store
fi

if [ $# -eq 2 ]; then
    taskset -c 1 ./benchmark.out $1 $2
else
    taskset -c 1 ./benchmark.out $1
fi
