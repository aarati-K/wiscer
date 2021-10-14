#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <cmath>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include "metrics.h"

using namespace std;

#ifndef _CHAINED_H_
#define _CHAINED_H_

#define FETCH_REQ 0
#define INSERT_REQ 1
#define DELETE_REQ 2

typedef struct HashmapReq {
    ulong key;
    ulong value;
    uint8_t reqType;
} HashmapReq;

typedef struct KV {
    ulong key;
    ulong value;
    struct KV* next;
} KV;

class Hashmap {
private:
    int hashpower;
    long cardinality;
    ulong hmsize;
    KV **dict;
    KV *entries;
    ulong entriesOffset = 0;
    ulong numReqs = 0;
    ulong displacement;
    struct timespec startTime, endTime;

public:
    Hashmap();
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
    void _setFinal(ulong, ulong);
    ulong _getTimeDiff(struct timespec, struct timespec);
    int _getHashpower();
};

#endif // _CHAINED_H_
