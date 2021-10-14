#!/bin/bash

mkdir -p output
make clean && make default && cp benchmark.out default.out
make clean && make vip && cp benchmark.out vip.out

echo "Workload: STATIC POPULARITY UNIFORM DISTRIBUTION"
echo "workloads/static_popularity_default_zipf0"
taskset -c 1 ./default.out workloads/static_popularity_default_zipf0
echo "-------------"
sleep 10
echo "workloads/static_popularity_adaptive_zipf0"
taskset -c 1 ./vip.out workloads/static_popularity_adaptive_zipf0
echo "-------------"
echo ""
sleep 10

echo "Workload: STATIC POPULARITY LOW SKEW"
echo "workloads/static_popularity_default_zipf1"
taskset -c 1 ./default.out workloads/static_popularity_default_zipf1
echo "-------------"
sleep 10
echo "workloads/static_popularity_adaptive_zipf1"
taskset -c 1 ./vip.out workloads/static_popularity_adaptive_zipf1
echo "-------------"
echo ""
sleep 10

echo "Workload: STATIC POPULARITY MEDIUM SKEW"
echo "workloads/static_popularity_default_zipf2"
taskset -c 1 ./default.out workloads/static_popularity_default_zipf2
echo "-------------"
sleep 10
echo "workloads/static_popularity_adaptive_zipf2"
taskset -c 1 ./vip.out workloads/static_popularity_adaptive_zipf2
echo "-------------"
sleep 10
echo ""

echo "Workload: MEDIUM POPULARITY CHURN"
echo "workloads/medium_churn_default"
taskset -c 1 ./default.out workloads/medium_churn_default
echo "-------------"
sleep 10
echo "workloads/medium_churn_adaptive"
taskset -c 1 ./vip.out workloads/medium_churn_adaptive
echo "-------------"
echo ""
sleep 10

echo "Workload: HIGH POPULARITY CHURN"
echo "workloads/high_churn_default"
taskset -c 1 ./default.out workloads/high_churn_default
echo "-------------"
sleep 10
echo "workloads/high_churn_adaptive"
taskset -c 1 ./vip.out workloads/high_churn_adaptive
echo "-------------"
echo ""
sleep 10

echo "Workload: STEADY STATE"
echo "workloads/steady_state_default"
taskset -c 1 ./default.out workloads/steady_state_default
echo "-------------"
sleep 10
echo "workloads/steady_state_adaptive"
taskset -c 1 ./vip.out workloads/steady_state_adaptive
echo "-------------"
echo ""
sleep 10

echo "Workload: READ MOSTLY"
echo "workloads/read_mostly_default"
taskset -c 1 ./default.out workloads/read_mostly_default
echo "-------------"
sleep 10
echo "workloads/read_mostly_adaptive"
taskset -c 1 ./vip.out workloads/read_mostly_adaptive
echo "-------------"
echo ""

make clean
