# Wiscer

Wiscer is a benchmarking tool for systematically generating point query (fetch/insert/delete) workloads, that can capture real-world behavior such as skew in popularity of keys and shifting set of hot keys over time. The workload can be issued to any storage engine that implements the interface specified in `hashmap.h`. Currently, the code is implemented for a Linux-based platform.

## Table of Contents
1. [Building & running Wiscer](#buildnrun)
    * [System Requirements](#sysreq)
2. [Configuring workloads](#workload)
3. [Sample workloads & scripts](#scripts)
4. [Measuring hardware metrics using the Intel PMU](#pmu)
5. [Good practices](#goodpractices)

## Building & Running Wiscer <a name="buildnrun"></a>

To build Wiscer, run

```
$ make
$ ./bechmark.out
Usage: ./benchmark.out workload_file_name (random_seed)
$ ./benchmark.out workloads/test
```

Operation throughput (and other hardware metrics if configured, see below) is measured for every batch of 1M requests, and the output is stored to file `output.txt` unless an `outputFile` parameter is specified in the workload. The directory `workloads/` contains multiple workload files for reference.

#### System Requirements <a name="sysreq"></a>

1. *Intel Intrinsics* - Intel Intrinsics libraries might not be available for some platforms, in which case the flag `_INTEL_INTRINSICS_` in file `chained_adaptive.h` should be disabled.\
`#define _INTEL_INTRINSICS_ 0`

2. *Memory Requirements* - Wiscer generates the operations to be issued to the hash table all at once, at the start before benchmarking the configured hash table. Thus, the system needs to have sufficient memory to hold the generated workload.

## Configuring Workloads <a name="workload"></a>

Currently, the workload configuration options that Wiscer supports are:

* `zipf` - Zipfian factor of the workload (zipf=0.0 corresponds to uniform distribution). Valid values are positive floating point.
* `initialSize` - Initial number of keys in the storage engine (hash table) before running any operations.
* `operationCount` - Number of operations to issue to the configured storage engine.
* `insertProportion` - Proportion of insert operations.
* `fetchProportion` - Proportion of fetch operations.
* `deleteProportion` - Proportion of delete operations.
* `distShiftFreq` - A shift in popularity distribution occurs after every `distShiftFreq` operations.
* `distShiftPrct` - The popularity distribution shifts by `distShiftPrct` every `distShiftFreq` operations. 50% shift corresponds to the top keys contributing to 50% of the probability falling out of the hot set and randomly being replaced by less popular keys.
* `storageEngine` - What storage engine to use. Options are `ChainedHashing`|`VIPHashing`|`none` (store workload to disk). Default is `ChainedHashing`.
* `keyPattern` - Pattern of keys to generate. Options are `random`|`sequential` (default: `random`).
* `keyOrder` - The popularity rank of keys relative to the initial insertion order. Options are `random`|`sorted` (the last inserted key is the most popular, a.k.a. latest). Default is `random`.
* `outputFile` - Where to store the output of the workload. Default is `output.txt`.

The test workload (file `workloads/test`) that we ran has the following configuration options:

```
zipf=1
initialSize=1048576
operationCount=500000000
distShiftFreq=500000001
distShiftPrct=0
fetchProportion=1
insertProportion=0
deleteProportion=0
storageEngine=VIPHashing
```

Stated in words, 500M fetch operations with low skew (`zipf` = 1) are issued to a hash table with 2<sup>20</sup> (= 1048576) keys. The popularity distribution is static (note that `distShiftFreq` > `operationCount`), and the hash table being benchmarked is `VIPHashing`.

## Sample Workloads & Scripts <a name="scripts"></a>

We have provided multiple workload files that capture different workload behavior:

* *Static* - The keys in the hash table and their popularity are static.
* *Popularity Churn* - There is churn in the set of most popular keys at different rates (medium & high)
* *Steady State* - Insert and delete operations are equal, and the number of keys remains approximately steady
* *Read Mostly* - Small percentage of insert operations are issued, causing the number of keys to grow over time.

Overall, a total of six workloads for each hash table implementation have been provided. To run all the workloads,

```
$ ./run_all.sh > results.txt
```

The full output with fine-grained metrics collected per-batch of 1M operations is stored in folder `output/`.

To parse the results obtained, first install the `tabluate` python libary:

```
$ sudo apt install python-tabulate
```
or
```
$ pip install tabulate
```

Then, run
```
$ python2 parse_results.py
```

## Measuring Hardware Metrics using the Intel PMU <a name="pmu"></a>

By default, measuring hardware metrics is disabled. The code for collecting hardware metrics is specific to Intel CPUs, and the performance monitoring unit (PMU) registers need to be programmed according to the processor's architecture family. The steps to follow are:

1. To access performance counting registers, enable `rdmpc` instruction at user level:\
`sudo echo 2 > /sys/devices/cpu/rdpmc`
2. Install required libraries:\
`sudo apt install cpuid msr-tools`
3. Create necessary module files:\
`sudo modprobe msr`
4. Build and install the [msr-tools](https://github.com/intel/msr-tools) repository from Intel (just installing `msr-tools` using `apt` is not sufficient):\
```
$ git clone https://github.com/intel/msr-tools.git
$ cd msr-tools && ./autogen.sh && ./MAKEDEV-cpuid-msr
```
5. For Intel Skylake architecture family, we can program the PMU of core ID `1` as follows:
    - To enable all PMU units on the core, set register IA32_PERF_GLOBAL_CTRL at 0x38f to 0x70000000f:\
    `wrmsr -p 1 0x38f 0x70000000f`
    - To enable all fixed counters (reference cycles, actual cycles, retired instructions, etc.), set register IA32_FIXED_CTR_CTRL MSR at 0x38d to 0x333:\
    `wrmsr -p 1 0x38d 0x333`
    - Program register IA32_PERFEVTSEL0 (0x186) to count L3 misses:\
    `wrmsr -p 1 0x186 0x43412e`
    - Program register IA32_PERFEVTSEL1 (0x187) to count L2 references (L1 misses):\
    `wrmsr -p 1 0x187 0x43ef24`
    - Program register IA32_PERFEVTSEL2 (0x188) to count L2 misses:\
    `wrmsr -p 1 0x188 0x433f24`
6. Enable counting hardware metrics by setting the flag `_COLLECT_METRICS_` in file `metrics.h` to `1`:\
`#define _COLLECT_METRICS_ 1`
7. Remember to run the code on the programmed core to measure hardware metrics:\
`taskset -c 1 ./benchmark.out workloads/test`
8. Some helpful resources:
    - Discussions on Intel Software Forum [(1)](https://software.intel.com/en-us/forums/software-tuning-performance-optimization-platform-monitoring/topic/783505) [(2)](https://software.intel.com/en-us/forums/software-tuning-performance-optimization-platform-monitoring/topic/595214)
    - [Intel's Developer Manual Volume 3](https://www.intel.com/content/www/us/en/architecture-and-technology/64-ia-32-architectures-software-developer-system-programming-manual-325384.html) Chapter 18 details how to program and use the PMU for different Intel architecture families.

## Good practices <a name="goodpractices"></a>

* *Dedicated server machine for benchmarking* - This avoids interference from concurrent processes.
* *Pin process to a core* - By pinning the process to a core, we can avoid the overhead of task switching if the chosen core is free. In our script `run.sh`, we pin the process to core `1` (avoid choosing core `0`).
* *Disable frequency scaling* - Set the scaling governor of the chosen core to `performance`:\
`sudo echo performance > /sys/devices/system/cpu/cpu1/cpufreq/scaling_governor`\
This mitigates variablity in performance due to changing processor frequency.
