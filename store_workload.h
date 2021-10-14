#ifdef STORE

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
#include <fstream>

using namespace std;

#define FETCH_REQ 0
#define INSERT_REQ 1
#define DELETE_REQ 2

#define _SORT_KEYS_ 0

typedef struct HashmapReq {
    ulong key;
    ulong value;
    uint8_t reqType;
} HashmapReq;

class StoreWorkload {
private:
    int hashpower;
    long cardinality;
    ulong hmsize;

    ofstream keys_file;
    ofstream ops_file;

    char fetchOpcode = 0;
    char insertOpcode = 1;
    char deleteOpcode = 2;
public:
    StoreWorkload();
    void initHashpower(int);
    void bulkLoad(ulong*, ulong);
    Metrics processRequests(HashmapReq*, ulong);
    void rehash();
    void free();

    void _heapify(ulong*, long, long);
    void _heapSort(ulong*, long);
};

typedef StoreWorkload Hashmap;

#endif // STORE
