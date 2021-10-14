#include "workload.h"

Workload::Workload(float zipf,
    ulong initialSize,
    ulong operationCount,
    ulong distShiftFreq,
    float distShiftPrct,
    float fetchProportion,
    float insertProportion,
    float deleteProportion,
    string storageEngine
) {
    this->zipf = zipf;
    this->initialSize = initialSize;
    this->operationCount = operationCount;
    this->distShiftFreq = distShiftFreq;
    this->distShiftPrct = distShiftPrct/10;
    this->fetchProportion = fetchProportion;
    this->insertProportion = insertProportion;
    this->deleteProportion = deleteProportion;
    this->hm = new Hashmap();
}

Workload::Workload(string filename) {
    ifstream inputFile(filename);
    string line;
    char *token, *property, *val;
    if (!inputFile.is_open()) {
        cout << "Could not open file " << filename << endl;
        return;
    }
    this->workloadFile = filename;
    while (getline(inputFile, line)) {
        if (!line.length()) continue;
        property = strtok((char*)line.c_str(), "=");;
        val = strtok(NULL, "=");
        if (strcmp(property, "zipf") == 0) {
            this->zipf = stof(val);
        } else if (strcmp(property, "initialSize") == 0) {
            this->initialSize = (ulong)stoul(val);
        } else if (strcmp(property, "operationCount") == 0) {
            this->operationCount = (ulong)stoul(val);
        } else if (strcmp(property, "distShiftFreq") == 0) {
            this->distShiftFreq = (ulong)stoul(val);
        } else if (strcmp(property, "distShiftPrct") == 0) {
            this->distShiftPrct = stof(val)/float(100);
        } else if (strcmp(property, "fetchProportion") == 0) {
            this->fetchProportion = stof(val);
        } else if (strcmp(property, "insertProportion") == 0) {
            this->insertProportion = stof(val);
        } else if (strcmp(property, "deleteProportion") == 0) {
            this->deleteProportion = stof(val);
        } else if (strcmp(property, "storageEngine") == 0) {
            this->hm = new Hashmap();
        } else if (strcmp(property, "keyPattern") == 0) {
            if (strcmp(val, "random") == 0) {
                this->keyPattern = RANDOM;
            } else if (strcmp(val, "sequential") == 0) {
                this->keyPattern = SEQUENTIAL;
            } else {
                this->keyPattern = RANDOM;
            }
        } else if (strcmp(property, "keyorder") == 0) {
            if (strcmp(val, "random") == 0) {
                this->keyorder = RANDOM;
            } else if (strcmp(val, "sorted") == 0) {
                this->keyorder = SORTED;
            } else {
                this->keyorder = RANDOM;
            }
        } else if (strcmp(property, "outputFile") == 0) {
            this->outputFile = val;
        }
    }
    if (this->hm == NULL) {
        this->hm = new Hashmap();
    }
}

void Workload::setRandomSeed(uint seed) {
    srand(seed);
}

void Workload::printParams() {
    cout << "zipf: " << this->zipf << endl;
    cout << "initialSize: " << this->initialSize << endl;
    cout << "operationCount: " << this->operationCount << endl;
    cout << "distShiftFreq: " << this->distShiftFreq << endl;
    cout << "distShiftPrct: " << this->distShiftPrct << endl;
    cout << "fetchProportion: " << this->fetchProportion << endl;
    cout << "insertProportion: " << this->insertProportion << endl;
    cout << "deleteProportion: " << this->deleteProportion << endl;
}

void Workload::run() {
    initHashmap();

    // Run
    HashmapReq *reqs = (HashmapReq*)malloc(sizeof(HashmapReq)*this->operationCount);
    if (!reqs) {
        cout << "Malloc failed while initializing workload, insufficient memory" << endl;
        exit(1);
    }
    memset(reqs, 0, sizeof(HashmapReq)*this->operationCount);
    ulong numBatches = this->operationCount/this->batchSize;
    if (this->operationCount % this->batchSize > 0) {
        numBatches += 1;
    }
    this->throughput = (ulong*)malloc(sizeof(ulong)*numBatches);
    this->retdInsts = (ulong*)malloc(sizeof(ulong)*numBatches);
    this->l3Misses = (ulong*)malloc(sizeof(ulong)*numBatches);
    this->l2Misses = (ulong*)malloc(sizeof(ulong)*numBatches);
    this->l1Misses = (ulong*)malloc(sizeof(ulong)*numBatches);
    this->displacement = (ulong*)malloc(sizeof(ulong)*numBatches);
    memset(this->throughput, 0, sizeof(ulong)*numBatches);
    float fetchSlab = this->fetchProportion*100;
    float deleteSlab = (this->fetchProportion + this->deleteProportion)*100;
    float insertSlab = (this->fetchProportion + this->deleteProportion +
        this->insertProportion)*100;
    if (insertSlab != 100) {
        cout << "Invalid request proportions" << endl;
        exit(1);
    }
    ulong r;
    Metrics m;
    ulong totalTime = 0;
    /* First generate the operations */
    for (ulong i = 0; i<this->operationCount; i++) {
        r = random() % 100;
        if (r < fetchSlab) {
            // req_type = FETCH_REQ;
            this->_genFetchReq(reqs, i);
        } else if (r < deleteSlab && r >= fetchSlab) {
            // req_type = DELETE_REQ;
            this->_genDeleteReq(reqs, i);
        } else if (r < insertSlab && r >= deleteSlab) {
            // req_type = INSERT_REQ;
            this->_genInsertReq(reqs, i);
        }
        requestsIssued += 1;
        if (requestsIssued % distShiftFreq == 0) {
            this->_shiftDist();
        }
    }
    for (ulong i=0; i<numBatches; i++) {
        ulong batchSize = this->batchSize;
        if (i == numBatches - 1 && (this->operationCount % this->batchSize) > 0) {
            batchSize = this->operationCount % this->batchSize;
        }
        m = hm->processRequests(&reqs[i*(this->batchSize)], batchSize); // us
        throughput[i] = (1000000*batchSize)/m.timeElapsedus;
        retdInsts[i] = m.retiredInst;
        l3Misses[i] = m.l3Miss;
        l2Misses[i] = m.l2Miss;
        l1Misses[i] = m.l1Miss;
        displacement[i] = m.displacement;
        totalTime += m.timeElapsedus;
        hm->rehash(); // Do rehashing if necessary
    }
    cout << "Total time (us): " << totalTime << endl;
    cout << "Net throughput: " << this->operationCount/float(totalTime) << " Mops/s" << endl;
    storeOutput();
    std::free(reqs);
    this->free();
}

inline void Workload::initHashmap() {
    int hashpower = 0;
    while (pow(2, hashpower) < this->initialSize) {
        hashpower += 1;
    }

    hm->initHashpower(hashpower);
    this->_choosePrime();

    ulong finalSize = this->initialSize + this->insertProportion*operationCount + 1;
    popOrder = (ulong*)malloc(sizeof(ulong)*finalSize);
    cumProb = (double*)malloc(sizeof(double)*finalSize);
    memset(cumProb, 0, sizeof(double)*finalSize);
    memset(popOrder, 0, sizeof(ulong)*finalSize);

    if (this->keyPattern == RANDOM) {
        popOrder[0] = this->_multAddHash(0);
        cumProb[0] = 1/pow(1, this->zipf);
        cumsum = cumProb[0];
        for (ulong i=1; i<initialSize; i++) {
            /* Mult Add hash function, for generating random ulong numbers */
            popOrder[i] = this->_multAddHash(i);
            cumProb[i] = 1/pow(i+1, this->zipf);
            cumsum += cumProb[i];
            cumProb[i] += cumProb[i-1];
        }
    } else { // SEQUENTIAL
        popOrder[0] = 0;
        cumProb[0] = 1/pow(1, this->zipf);
        cumsum = cumProb[0];
        for (ulong i=1; i<initialSize; i++) {
            popOrder[i] = i;
            cumProb[i] = 1/pow(i+1, this->zipf);
            cumsum += cumProb[i];
            cumProb[i] += cumProb[i-1];
        }
        // The popularity order of keys is arbitrary
        this->_random_shuffle(popOrder, initialSize);
    }

    hm->bulkLoad(popOrder, initialSize);
    this->cardinality = initialSize;
    this->maxInsertedIdx = initialSize-1;
    if (this->keyorder == RANDOM) {
        this->_random_shuffle(popOrder, initialSize);
    } else if (this->keyorder == SORTED) {
        // key inserted at the end is in the most favorable spot
        this->_reverse(popOrder, initialSize);
    }
}

void Workload::storeOutput() {
    ofstream output(this->outputFile);
    ulong numBatches = this->operationCount/this->batchSize;
    if (this->operationCount % this->batchSize > 0) {
        numBatches += 1;
    }
    output << "Throughput | "
        << "Retd. Instructions | "
        << "L3 Misses | "
        << "L2 Misses | "
        << "L1 Misses | "
        << "Displacement" << endl;
    for (ulong i=0; i<numBatches; i++) {
        output << throughput[i] << " " 
            << retdInsts[i] << " "
            << l3Misses[i] << " "
            << l2Misses[i] << " "
            << l1Misses[i] << " "
            << displacement[i]
            << endl;
    }
    output.close();
}

inline void Workload::_genFetchReq(HashmapReq *reqs, ulong i) {
    ulong r = this->_random();
    ulong max = 4611686018427387903L;
    double slab = double(r)/double(max);
    slab = slab*this->cumsum;
    ulong low = 0;
    ulong high = this->cardinality;
    ulong mid;

    while ((high - low) > 1){
        // cout << "high: " << high << ", low: " << low << ", mid: " << mid << endl;
        if (slab < cumProb[0]) {
            high = 0;
            break;
        } else if (slab > cumProb[this->cardinality-1]) {
            r = this->_random();
            slab = double(r)/double(max);
            slab = slab*this->cumsum;
            continue;
        }
        mid = (low + high)/2;
        if (this->cumProb[mid] > slab) {
            high = mid;
        } else if (this->cumProb[mid] <= slab) {
            low = mid;
        }
    }
    // Set req params
    reqs[i].key = this->popOrder[high];
    reqs[i].value = this->_random();
    reqs[i].reqType = FETCH_REQ;
}

inline void Workload::_genInsertReq(HashmapReq *reqs, ulong i) {
    ulong n = _multAddHash(this->maxInsertedIdx + 1);
    this->maxInsertedIdx += 1;

    // set the request
    reqs[i].key = n;
    reqs[i].value = _random();
    reqs[i].reqType = INSERT_REQ;
    
    ulong p = this->_random() % this->cardinality;
    ulong buf;
    while (cumProb[p]/cumsum < 0.9) {
        buf = this->popOrder[p];
        this->popOrder[p] = n;
        n = buf;
        p = this->_random() % (this->cardinality - 1 - p) + p + 1;
    }
    // For the last 10% keys, it doesn't matter. Just insert at the end.
    this->popOrder[this->cardinality] = n;

    // set state tracking params
    this->cardinality += 1;
    if (this->cumProb[this->cardinality-1] == 0) {
        this->cumProb[this->cardinality-1] = 1/pow(this->cardinality, zipf);
        this->cumProb[this->cardinality-1] += this->cumProb[this->cardinality-2];
    }
    this->cumsum += this->cumProb[this->cardinality-1] - this->cumProb[this->cardinality-2];
}

inline void Workload::_genDeleteReq(HashmapReq *reqs, ulong i) {
    ulong p = this->_random() % this->cardinality;
    ulong p_old, buf;

    // set the request, popOrder[p] has the key to be deleted
    reqs[i].key = popOrder[p];
    reqs[i].value = 0;
    reqs[i].reqType = DELETE_REQ;

    // bubble down
    while (cumProb[p]/cumsum < 0.9) {
        p_old = p;
        buf = popOrder[p];
        p = this->_random() % (this->cardinality - p - 1) + p + 1;
        this->popOrder[p_old] = this->popOrder[p];
        this->popOrder[p] = buf;
    }

    // For the last 10% prob, just exchange with the last element
    popOrder[p] = popOrder[this->cardinality-1];

    // set state tracking params
    cumsum -= (cumProb[this->cardinality-1] - cumProb[this->cardinality-2]);
    this->cardinality -= 1;
}

inline void Workload::_shiftDist() {
    if (this->distShiftPrct == 0) return;
    ulong p = -1;
    ulong pnew, buf;
    do {
        p += 1;
        pnew = this->_random() % (this->cardinality - p - 1) + p + 1;
        buf = this->popOrder[p];
        popOrder[p] = popOrder[pnew];
        popOrder[pnew] = buf;
    } while (this->cumProb[p]/this->cumsum < this->distShiftPrct);
}

void Workload::free() {
    this->zipf = 1;
    this->initialSize = 0;
    this->operationCount = 0;
    this->distShiftFreq = 0;
    this->distShiftPrct = 0;
    this->fetchProportion = 1;
    this->insertProportion = 0;
    this->deleteProportion = 0;
    std::free((void*)this->popOrder);
    std::free((void*)this->cumProb);
    std::free((void*)this->throughput);
    std::free((void*)this->retdInsts);
    std::free((void*)this->l3Misses);
    std::free((void*)this->l2Misses);
    std::free((void*)this->l1Misses);
    std::free((void*)this->displacement);
    hm->free();
}

inline ulong Workload::_multAddHash(ulong i) {
    return i*this->chosenPrime[0] + this->chosenPrime[1];;
}

void Workload::_choosePrime() {
    // srand(0);
    ulong primes[10] = {7761362401859887,
        5937911637806579,
        4062741855079417,
        6757727681041201,
        1767830588689157,
        6029750347057651,
        9370386929117087,
        5158899522772007,
        6846396768799417,
        6411045436602071};
    int i = random() % 10;
    this->chosenPrime[0] = primes[i];

    do {
        i = random() % 10;
        this->chosenPrime[1] = primes[i];
    } while (this->chosenPrime[0] == this->chosenPrime[1]);
}

inline ulong Workload::_random() {
    ulong r = (ulong)random();
    r = r<<31;
    r = r + (ulong)random();
    return r;
}

void Workload::_random_shuffle(ulong *array, ulong len) {
    ulong pos;
    ulong swap;
    for (int j=0; j<3; j++) {
        for (ulong i=len-1; i>0; i--) {
            pos = this->_random()%(i+1);
            swap = array[i];
            array[i] = array[pos];
            array[pos] = swap;
        }
    }
}

void Workload::_reverse(ulong *array, ulong len) {
    ulong start = 0;
    ulong end = len - 1;
    ulong buf;
    while (start < end) {
        buf = array[start];
        array[start] = array[end];
        array[end] = buf;
        start += 1;
        end -= 1;
    }
}
