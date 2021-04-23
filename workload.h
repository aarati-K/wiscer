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
#include "hashmap.h"
#include "chained.h"

using namespace std;

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
    float updateProportion = 0;
    float deleteProportion = 0;

    Hashmap *hm;
    string workloadFile;
    ulong *popOrder;
    double *cumProb;
    double cumsum;
    ulong cardinality = 0;
    ulong maxInsertedIdx = 0;
    ulong requestsIssued = 0;
    ulong *throughput;

    // Random prime numbers to choose from
    ulong chosenPrime[2];
    ulong batchSize = 100000; // 100k requests at a time
    ulong _multAddHash(ulong);
    void _choosePrime();
    ulong _random();
    void _random_shuffle(ulong*, ulong);
    void _genFetchReq(HashmapReq*, ulong);
    void _genInsertReq(HashmapReq*, ulong);
    void _genUpdateReq(HashmapReq*, ulong);
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
        float updateProportion,
        float deleteProportion
    );
    Workload(string filename);
    void printParams();
    void initHashmap();
    void storeOutput();
    void run();
    void free();
};
