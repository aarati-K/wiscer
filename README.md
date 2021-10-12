# Wiscer

Wiscer is a benchmarking tool for systematically generating point query (fetch/insert/delete) workloads, that simulates real-world behavior such as skew in popularity of keys and shifting set of hot keys over time. The workload can be issued to any storage engine that implements the interface specified in `hashmap.h`. Currently, the code is implemented for a Linux-based platform.

## Building & Running Wiscer

To build Wiscer, run

```
$ make
$ ./bechmark.out
Usage: ./benchmark.out workload_file_name (random_seed)
$ ./benchmark.out workloads/test
```

Operation throughput (and other hardware metrics if configured, see below) is measured every batch of 1M requests, and the output is stored to file `output.txt` unless an `outputFile` parameter is specified in the workload. The directory `workloads` contains multiple workload files for reference. Currently, the workload configuration options that Wiscer supports are:

* `zipf` - zipf factor of the workload (zipf=0.0 corresponds to uniform distribution). Valid values include any positive float.
* `initialSize` - Initial number of keys in the hash table (storage engine) before running any operations.
* `operationCount` - Number of operations to issue to the configured storage engine.
* `insertProportion` - Proportion of insert requests.
* `fetchProportion` - Proportion of fetch requests.
* `deleteProportion` - Proportion of delete requests.
* `distShiftFreq` - A shift in popularity distribution occurs after every `distShiftFreq` operations.
* `distShiftPrct` - By how much the distribution shifting. 50% shift corresponds to the top keys contributing to 50% of the probability falling out of the hot set and being replaced by less popular keys.
* `storageEngine` - What storage engine to use. Options are `ChainedHashing`|`VIPHashing|none` (default: `ChainedHashing`).
* `keyPattern` - Pattern of keys to generate. Options are `random`|`sequential` (default: `random`).
* `keyOrder` - The popularity rank of keys relative to the insertion order. Options are `random`|`sorted` (a.k.a. latest). Default is `random`.
* `outputFile` - Where to store the output of the workload. Default is `output.txt`.

*Note on memory requirements* - Wiscer generates the operations to be issued to the hash table all at once, at the start before benchmarking the configured hash table. Thus, the system needs to have sufficient memory to hold the generated workload.

## Scripts

## Measuring Hardware Metrics using the PMU

## Good practices
