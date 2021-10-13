#include <time.h>
#include <sys/time.h>
#include <sys/types.h>

#ifndef _METRICS_H_
#define _METRICS_H_

// Change this to 1 to collect metrics
#define _COLLECT_METRICS_ 0
#define _COUNT_DISP_ 1

typedef struct Metrics {
    struct timespec startTime;
    struct timespec endTime;
    ulong timeElapsedus;
    ulong retiredInst;
    ulong l3Miss;
    ulong l2Miss;
    ulong l1Miss;
    ulong displacement;
} Metrics;

unsigned long getTimeDiff(struct timespec, struct  timespec);
unsigned long rdpmc_retired_inst_all();
unsigned long rdpmc_llc_cache_misses();
unsigned long rdpmc_l2_cache_misses();
unsigned long rdpmc_l2_cache_refs();
void getMetricsStart(Metrics&);
void getMetricsEnd(Metrics&);

#endif // _METRICS_H_
