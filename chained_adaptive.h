#include "hashmap.h"
#include "chained.h"

using namespace std;

#ifndef _CHAINED_ADAPTIVE_H_
#define _CHAINED_ADAPTIVE_H_

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
    // ulong accessesOffset = 0; // should be entriesOffset + 1
    ulong numReqs = 0;
    struct timespec startTime, endTime;

    bool adaptiveOn = false;
    bool sensingOn = false;
    bool benchmarkingOn = false;

    // Algorithm configuration
    int periodicity = 40;
    int epoch_size_factor = 1;
    double confidence_prct = 0.95;
    int sample_size = 1000;

    double u0, v0, w0; // estimated when benchmarking
    double u1, v1, w1; // estimated when sensing 

public:
    ChainedAdaptive();
    void initHashpower(int);
    void bulkLoad(ulong*, ulong);
    ulong processRequests(HashmapReq*, ulong);
    void rehash();
    void free();

    // helper functions
    ulong _random();
    ulong _murmurHash(ulong);
    void _fetch(HashmapReq*);
    void _insert(HashmapReq*);
    void _delete(HashmapReq*);
    void _update(HashmapReq*);
    void _setFinal(ulong, ulong);
    ulong _getTimeDiff(struct timespec, struct timespec);
    int _getHashpower();
    void _resetAccesses();
};

#endif // _CHAINED_ADAPTIVE_H_
