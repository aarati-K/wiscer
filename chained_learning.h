#include "hashmap.h"
#include <fstream>
#include <stack>

using namespace std;

#ifndef _CHAINED_LEARNING_H_
#define _CHAINED_LEARNING_H_

typedef struct KV {
    ulong key;
    ulong value;
    uint accesses;
    struct KV* next;
} KV;

class ChainedHashmap : public Hashmap {
private:
    KV **dict;
    KV *entries;
    ulong entriesOffset = 0;
    ulong numReqs = 0;
    ulong displacement;
    struct timespec startTime, endTime;

public:
    ChainedHashmap();
    void initHashpower(int);
    void bulkLoad(ulong*, ulong);
    Metrics processRequests(HashmapReq*, ulong);
    void rehash();
    void free();
    void storeHashmap();

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
};

#endif // _CHAINED_LEARNING_H_
