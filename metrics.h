#include <time.h>
#include <sys/time.h>
#include <sys/types.h>

using namespace std;

#ifndef _METRICS_H_
#define _METRICS_H_

typedef struct Metrics {
    struct timespec startTime;
    struct timespec endTime;
    ulong timeElapsedus;
    ulong retiredInst;
    ulong l3Miss;
    ulong l2Miss;
    ulong l1Miss;
} Metrics;

unsigned long getTimeDiff(struct timespec, struct  timespec);
unsigned long rdpmc_retired_inst_all();
unsigned long rdpmc_llc_cache_misses();
unsigned long rdpmc_l2_cache_misses();
unsigned long rdpmc_l2_cache_refs();
void getMetricsStart(Metrics&);
void getMetricsEnd(Metrics&);

#endif // _METRICS_H_