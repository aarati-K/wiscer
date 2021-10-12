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

#ifndef _HASHMAP_H_
#define _HASHMAP_H_

#define FETCH_REQ 0
#define INSERT_REQ 1
#define DELETE_REQ 2

typedef struct HashmapReq {
    ulong key;
    ulong value;
    uint8_t reqType;
} HashmapReq;

class Hashmap {
protected:
    int hashpower;
    long cardinality;
    ulong hmsize;

public:
    Hashmap() {}
    virtual void initHashpower(int) = 0;
    virtual void bulkLoad(ulong*, ulong) = 0;
    virtual Metrics processRequests(HashmapReq*, ulong) = 0;
    virtual void rehash() = 0;
    virtual void free() = 0;
};

#endif // _HASHMAP_H_
