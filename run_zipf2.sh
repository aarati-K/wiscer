#!/bin/bash

mkdir -p output
make clean
make default && cp benchmark.out default.out
make vip && cp benchmark.out vip.out

echo "Workload: STATIC POPULARITY UNIFORM DISTRIBUTION (zipf=0)"
echo "workloads/static_popularity_default_zipf0"
taskset -c 1 ./default.out workloads/static_popularity_default_zipf0
echo "-------------"
sleep 10
echo "workloads/static_popularity_adaptive_zipf0"
taskset -c 1 ./vip.out workloads/static_popularity_adaptive_zipf0
echo "-------------"
echo ""
sleep 10

echo "Workload: STATIC POPULARITY MEDIUM SKEW (zipf=2.0)"
echo "workloads/static_popularity_default_zipf2"
taskset -c 1 ./default.out workloads/static_popularity_default_zipf2
echo "-------------"
sleep 10
echo "workloads/static_popularity_adaptive_zipf2"
taskset -c 1 ./vip.out workloads/static_popularity_adaptive_zipf2
echo "-------------"
sleep 10
echo ""

echo "Workload: MEDIUM POPULARITY CHURN (zipf=2.0)"
echo "workloads/medium_churn_default_zipf2"
taskset -c 1 ./default.out workloads/medium_churn_default_zipf2
echo "-------------"
sleep 10
echo "workloads/medium_churn_adaptive_zipf2"
taskset -c 1 ./vip.out workloads/medium_churn_adaptive_zipf2
echo "-------------"
echo ""
sleep 10

echo "Workload: HIGH POPULARITY CHURN (zipf=2.0)"
echo "workloads/high_churn_default_zipf2"
taskset -c 1 ./default.out workloads/high_churn_default_zipf2
echo "-------------"
sleep 10
echo "workloads/high_churn_adaptive_zipf2"
taskset -c 1 ./vip.out workloads/high_churn_adaptive_zipf2
echo "-------------"
echo ""
sleep 10

echo "Workload: STEADY STATE (zipf=2.0)"
echo "workloads/steady_state_default_zipf2"
taskset -c 1 ./default.out workloads/steady_state_default_zipf2
echo "-------------"
sleep 10
echo "workloads/steady_state_adaptive_zipf2"
taskset -c 1 ./vip.out workloads/steady_state_adaptive_zipf2
echo "-------------"
echo ""
sleep 10

echo "Workload: READ MOSTLY (zipf=2.0)"
echo "workloads/read_mostly_default_zipf2"
taskset -c 1 ./default.out workloads/read_mostly_default_zipf2
echo "-------------"
sleep 10
echo "workloads/read_mostly_adaptive_zipf2"
taskset -c 1 ./vip.out workloads/read_mostly_adaptive_zipf2
echo "-------------"
echo ""

make clean
