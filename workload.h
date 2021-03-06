#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <fstream>
#include <cmath>
#include "storage_engine.cpp"
#include "metrics.h"

using namespace std;

#ifndef _WORKLOAD_H_
#define _WORKLOAD_H_

// Initial key popularity orders
#define RANDOM  0
#define SORTED  1

// Key patterns
// #define RANDOM 0 (default)
#define SEQUENTIAL 1

class Workload {
private:
    // Params
    float zipf = 1;
    ulong initialSize = 0;
    ulong operationCount = 0;
    ulong distShiftFreq = 0;
    float distShiftPrct = 0;
    float fetchProportion = 1;
    float insertProportion = 0;
    float deleteProportion = 0;
    int keyPattern = RANDOM;
    int keyorder = RANDOM;
    string outputFile = "output.txt";

    Hashmap *hm;
    string workloadFile;
    ulong *popOrder;
    double *cumProb;
    double cumsum;
    ulong cardinality = 0;
    ulong maxInsertedIdx = 0;
    ulong requestsIssued = 0;

    // Metrics
    ulong *throughput;
    ulong *retdInsts;
    ulong *l3Misses;
    ulong *l2Misses;
    ulong *l1Misses;
    ulong *displacement;

    // Random prime numbers to choose from
    ulong chosenPrime[2];
    ulong batchSize = 1000000; // 1M requests at a time
    ulong _multAddHash(ulong);
    void _choosePrime();
    ulong _random();
    void _random_shuffle(ulong*, ulong);
    void _reverse(ulong*, ulong);
    void _genFetchReq(HashmapReq*, ulong);
    void _genInsertReq(HashmapReq*, ulong);
    void _genDeleteReq(HashmapReq*, ulong);
    void _shiftDist();

public:
    Workload(float zipf,
        ulong initialSize,
        ulong operationCount,
        ulong distShiftFreq,
        float distShiftPrct,
        float fetchProportion,
        float insertProportion,
        float deleteProportion,
        string storageEngine
    );
    Workload(string filename);
    void setRandomSeed(uint);
    void printParams();
    void initHashmap();
    void storeOutput();
    void run();
    void free();
};

#endif // _WORKLOAD_H_
