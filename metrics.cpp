#include "metrics.h"

using namespace std;

unsigned long getTimeDiff(struct timespec start_time, struct timespec end_time) {
    return (unsigned long)((end_time.tv_sec - start_time.tv_sec)*1000000 +
        double(end_time.tv_nsec - start_time.tv_nsec)/1000);
    return 0;
}

unsigned long rdpmc_retired_inst_all() {
#if _COLLECT_METRICS_
    unsigned long a, d, c;
    c = (1<<30);
    __asm__ __volatile__("rdpmc" : "=a" (a), "=d" (d) : "c" (c));
    return ((unsigned long)a) | (((unsigned long)d) << 32);
#else
    return 0;
#endif
}

// This gives L3 cache misses
unsigned long rdpmc_llc_cache_misses() {
#if _COLLECT_METRICS_
    // LONGEST_LAT_CACHE.MISS
    // mask:event = 0x43412e in 0x186
    unsigned long a, d, c;
    c = 0;
    __asm__ __volatile__("rdpmc" : "=a" (a), "=d" (d) : "c" (c));
    return ((unsigned long)a) | (((unsigned long)d) << 32);
#else
    return 0;
#endif
}

// This gives L2 cache references
// These are basically L1 misses
unsigned long rdpmc_l2_cache_refs() {
#if _COLLECT_METRICS_
    // L2_RQSTS.REFERENCES
    // mask:event = 0x43ef24 in 0x187
    unsigned long a, d, c;
    c = 1;
    __asm__ __volatile__("rdpmc" : "=a" (a), "=d" (d) : "c" (c));
    return ((unsigned long)a) | (((unsigned long)d) << 32);
#else
    return 0;
#endif
}

// This gives L2 cache misses
unsigned long rdpmc_l2_cache_misses() {
#if _COLLECT_METRICS_
    // L2_RQSTS.MISS
    // mask:event = 0x433f24 in 0x188
    unsigned long a, d, c;
    c = 2;
    __asm__ __volatile__("rdpmc" : "=a" (a), "=d" (d) : "c" (c));
    return ((unsigned long)a) | (((unsigned long)d) << 32);
#else
    return 0;
#endif
}

void getMetricsStart(Metrics &m) {
    clock_gettime(CLOCK_MONOTONIC, &(m.startTime));
    m.l3Miss = rdpmc_llc_cache_misses();
    m.l2Miss = rdpmc_l2_cache_misses();
    m.l1Miss = rdpmc_l2_cache_refs();
    m.retiredInst = rdpmc_retired_inst_all();
}

void getMetricsEnd(Metrics &m) {
    m.retiredInst = rdpmc_retired_inst_all() - m.retiredInst;
    m.l1Miss = rdpmc_l2_cache_refs() - m.l1Miss;
    m.l2Miss = rdpmc_l2_cache_misses() - m.l2Miss;
    m.l3Miss = rdpmc_llc_cache_misses() - m.l3Miss;
    clock_gettime(CLOCK_MONOTONIC, &(m.endTime));
    m.timeElapsedus = getTimeDiff(m.startTime, m.endTime);
}
