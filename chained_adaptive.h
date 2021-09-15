#include "hashmap.h"
#include "chained.h"
#include "x86intrin.h"

using namespace std;

#ifndef _CHAINED_ADAPTIVE_H_
#define _CHAINED_ADAPTIVE_H_

#define ADAPTIVE 0
#define BENCHMARKING 1
#define DEFAULT 2
#define SENSING 3

typedef struct Acc
{
    uint8_t accesses = 0;
    struct Acc* next;
} Acc;

class ChainedAdaptive : public Hashmap {
private:
    KV **dict;
    KV *entries;
    Acc **accessesDict;
    Acc *accesses;
    ulong entriesOffset = 0;
    ulong accessesOffset = 0;
    ulong numReqs = 0;
    ulong numReqsSlab = 0;
    struct timespec startTime, endTime;

    int mode = -1;

    // Algorithm configuration
    int periodicity = 60;
    int epoch_size_factor = 1;
    double confidence_prct = 0.95;
    int sample_size = 1000;
    ulong displacement = 0;
    ulong displacement_sq = 0;
    ulong statReqCount = 0;
    ulong displacementMetric = 0;

    // Algorithm parameters
    ulong epochSize = 0;
    double u0, v0, w0; // estimated when benchmarking
    double u1, v1, w1; // estimated when sensing 

public:
    ChainedAdaptive();
    void initHashpower(int);
    void bulkLoad(ulong*, ulong);
    Metrics processRequests(HashmapReq*, ulong);
    void rehash();
    void free();

    // helper functions
    ulong _random();
    ulong _murmurHash(ulong);
    void _fetchDefault(HashmapReq*);
    void _fetchAdaptive(HashmapReq*);
    void _fetchBenchmark(HashmapReq*);
    void _insert(HashmapReq*);
    void _delete(HashmapReq*);
    void _update(HashmapReq*);
    void _setFinal(ulong, ulong);
    void _setFinalEnd(ulong, ulong);
    ulong _getTimeDiff(struct timespec, struct timespec);
    int _getHashpower();
    void _resetAccesses();
    void _clearCache();
};

#endif // _CHAINED_ADAPTIVE_H_
