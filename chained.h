#include "hashmap.h"

using namespace std;

#ifndef _CHAINED_H_
#define _CHAINED_H_

typedef struct KV {
    ulong key;
    ulong value;
    struct KV* next;
} KV;

class ChainedHashmap : public Hashmap {
private:
    KV **dict;
    KV *entries;
    ulong entriesOffset = 0;
    ulong numReqs = 0;
    struct timespec startTime, endTime;

public:
    ChainedHashmap();
    void initHashpower(int);
    void bulkLoad(ulong*, ulong);
    Metrics processRequests(HashmapReq*, ulong);
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
};

#endif // _CHAINED_H_
