# Wiscer

Wiscer is a benchmarking tool for systematically generating skewed workloads and simulating behavior such as changing hot keys over time. The workload can be issued to any storage engine that implements the interface specified in `hashmap.h`. Currently, the workload properties wiscer supports are as follows:

* `initialSize` - Initial size of the storage engine
* `operationCount` - Number of operations to issue to the storage engine
* `zipf` - zipf factor of the workload (zipf=0.0 corresponds to uniform distribution). Valid numbers include any positive float 
* `distShiftFreq` - Number of operations when distribution shift occurs
* `distShiftPrct` - By how much the distribution shifting. 50% shift corresponds to the top keys contributing to 50% of the probability falling out of the hot set and being replaced by less popular keys.
* `insertProportion` - Proportion of insert requests
* `fetchProportion` - Proportion of fetch requests
* `deleteProportion` - Proportion of delete requests
* `updateProportion` - Proportion of update requests
* `storageEngine` - What storage engine to use. Options are `chained`|`linearProbing` (default: `chained`)
