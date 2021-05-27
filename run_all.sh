#!/bin/bash

make clean

# STATIC POPULARITY
# Default
make default
taskset -c 39 ./benchmark.out workloads/static_popularity_default_zipf0
echo "-------------"
sleep 10
taskset -c 39 ./benchmark.out workloads/static_popularity_default_zipf1
echo "-------------"
sleep 10
taskset -c 39 ./benchmark.out workloads/static_popularity_default_zipf2
echo "-------------"
sleep 10

# Adaptive
make adaptive
taskset -c 39 ./benchmark.out workloads/static_popularity_adaptive_zipf0
echo "-------------"
sleep 10
taskset -c 39 ./benchmark.out workloads/static_popularity_adaptive_zipf1
echo "-------------"
sleep 10
taskset -c 39 ./benchmark.out workloads/static_popularity_adaptive_zipf2
echo "-------------"
sleep 10

# POPULARITY CHURN
make default
taskset -c 39 ./benchmark.out workloads/medium_churn_default
echo "-------------"
sleep 10
taskset -c 39 ./benchmark.out workloads/high_churn_default
echo "-------------"
sleep 10

make adaptive
taskset -c 39 ./benchmark.out workloads/medium_churn_adaptive
echo "-------------"
sleep 10
taskset -c 39 ./benchmark.out workloads/high_churn_adaptive
echo "-------------"
sleep 10

# STEADY STATE
make default
taskset -c 39 ./benchmark.out workloads/steady_state_default
echo "-------------"
sleep 10
make adaptive
taskset -c 39 ./benchmark.out workloads/steady_state_adaptive
echo "-------------"
sleep 10

# READ MOSTLY

# READ LATEST
