#!/bin/bash

make clean
mkdir -p output

echo "Workload: STATIC POPULARITY UNIFORM DISTRIBUTION"
make default
echo "workloads/static_popularity_default_zipf0"
taskset -c 1 ./benchmark.out workloads/static_popularity_default_zipf0
echo "-------------"
sleep 10
make adaptive
echo "workloads/static_popularity_adaptive_zipf0"
taskset -c 1 ./benchmark.out workloads/static_popularity_adaptive_zipf0
echo "-------------"
sleep 10
echo ""

echo "Workload: STATIC POPULARITY LOW SKEW"
make default
echo "workloads/static_popularity_default_zipf1"
taskset -c 1 ./benchmark.out workloads/static_popularity_default_zipf1
echo "-------------"
sleep 10
make adaptive
echo "workloads/static_popularity_adaptive_zipf1"
taskset -c 1 ./benchmark.out workloads/static_popularity_adaptive_zipf1
echo "-------------"
sleep 10
echo ""

echo "Workload: STATIC POPULARITY MEDIUM SKEW"
make default
echo "workloads/static_popularity_default_zipf2"
taskset -c 1 ./benchmark.out workloads/static_popularity_default_zipf2
echo "-------------"
sleep 10
make adaptive
echo "workloads/static_popularity_adaptive_zipf2"
taskset -c 1 ./benchmark.out workloads/static_popularity_adaptive_zipf2
echo "-------------"
sleep 10
echo ""

echo "Workload: MEDIUM POPULARITY CHURN"
make default
echo "workloads/medium_churn_default"
taskset -c 1 ./benchmark.out workloads/medium_churn_default
echo "-------------"
sleep 10
make adaptive
echo "workloads/medium_churn_adaptive"
taskset -c 1 ./benchmark.out workloads/medium_churn_adaptive
echo "-------------"
sleep 10
echo ""

echo "Workload: HIGH POPULARITY CHURN"
make default
echo "workloads/high_churn_default"
taskset -c 1 ./benchmark.out workloads/high_churn_default
echo "-------------"
sleep 10
make adaptive
echo "workloads/high_churn_adaptive"
taskset -c 1 ./benchmark.out workloads/high_churn_adaptive
echo "-------------"
sleep 10
echo ""

echo "Workload: STEADY STATE"
make default
echo "workloads/steady_state_default"
taskset -c 1 ./benchmark.out workloads/steady_state_default
echo "-------------"
sleep 10
make adaptive
echo "workloads/steady_state_adaptive"
taskset -c 1 ./benchmark.out workloads/steady_state_adaptive
echo "-------------"
sleep 10
echo ""

echo "Workload: READ MOSTLY"
make default
echo "workloads/read_mostly_default"
taskset -c 1 ./benchmark.out workloads/read_mostly_default
echo "-------------"
sleep 10
make adaptive
echo "workloads/read_mostly_adaptive"
taskset -c 1 ./benchmark.out workloads/read_mostly_adaptive
echo "-------------"
sleep 10
echo ""
