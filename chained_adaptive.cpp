#include "chained_adaptive.h"

Hashmap::Hashmap() {
    this->hashpower = -1; // not initialized
    this->hmsize = 0;
    this->cardinality = 0;
    this->dict = NULL;
    this->entries = NULL;
    this->accessesDict = NULL;
    this->accesses = NULL;
    this->entriesOffset = 0;
    this->accessesOffset = 0;
    this->u0 = this->v0 = this->w0 = -1;
    this->u1 = this->v1 = this->w1 = -1;
    this->mode = ADAPTIVE;
}

void Hashmap::initHashpower(int hashpower) {
    this->hashpower = hashpower;
    this->hmsize = pow(2, hashpower);
    this->dict = (KV**)malloc(sizeof(KV*)*hmsize);
    this->entries = (KV*)malloc(sizeof(KV)*hmsize*20); // safety factor
    this->accessesDict = (Acc**)malloc(sizeof(Acc*)*hmsize);
    this->accesses = (Acc*)malloc(sizeof(Acc)*hmsize*20); // safety factor
    memset(this->dict, 0, sizeof(KV*)*hmsize);
    memset(this->entries, 0, sizeof(KV)*hmsize*20);
    memset(this->accessesDict, 0, sizeof(Acc*)*hmsize);
    memset(this->accesses, 0, sizeof(Acc)*(hmsize*20));
    entriesOffset = 0;
    accessesOffset = 0;
    epochSize = hmsize*float(epoch_size_factor);
    mode = ADAPTIVE;
    numReqsSlab = epochSize;
    displacement = 0;
    displacement_sq = 0;
    statReqCount = 0;
}

void Hashmap::bulkLoad(ulong *keys, ulong num_keys) {
    for (ulong i=0; i<num_keys; i++) {
        _setFinal(keys[i], _random());
    }
    this->cardinality = num_keys;
}

Metrics Hashmap::processRequests(HashmapReq *reqs, ulong count) {
    Metrics m;
    m.displacement = displacementMetric;
    getMetricsStart(m);
    ulong i = 0, j;
    ulong countReal = count;
    // make this a multiple of 10, to avoid inner for loop check for i < count
    count = count - (count % 10);
    while(i < count) {
        if (mode == ADAPTIVE) {
            while (numReqs < numReqsSlab && i<count) {
                for (j=0; j<10; j++,i++) {
                    switch (reqs[i].reqType) {
                        case FETCH_REQ:
                            _fetchAdaptive(&reqs[i]);
                            break;
                        case INSERT_REQ:
                            _insert(&reqs[i]);
                            break;
                        case DELETE_REQ:
                            _delete(&reqs[i]);
                            break;
                        default:
                            break;
                    }
                }
            }
            if (i == count) break;
            numReqsSlab += sample_size;
            _resetAccesses();
            _clearCache();
            mode = BENCHMARKING;
            displacement = 0;
            displacement_sq = 0;
            statReqCount = 0;
        }
        if (mode == BENCHMARKING) {
            while (numReqs < numReqsSlab && i<count) {
                for (j=0; j<10; j++, i++) {
                    switch (reqs[i].reqType) {
                        case FETCH_REQ:
                            _fetchBenchmark(&reqs[i]);
                            break;
                        case INSERT_REQ:
                            _insert(&reqs[i]);
                            break;
                        case DELETE_REQ:
                            _delete(&reqs[i]);
                            break;
                        default:
                            break;
                    }
                }
            }
            if (i == count) break;
            // Calculate u0, v0, w0
            u0 = double(displacement)/double(statReqCount);
            v0 = double(displacement_sq)/double(statReqCount-1) -
                double(displacement*displacement)/(double(statReqCount)*double(statReqCount-1));
            w0 = sqrt(((-1*log(1-confidence_prct))*2*v0)/statReqCount);
            numReqsSlab += epochSize*(periodicity - 1);
            mode = DEFAULT;
        }
        if (mode == DEFAULT) {
            while (numReqs < numReqsSlab && i<count) {
                for (j=0; j<10; j++, i++) {
                    switch (reqs[i].reqType) {
                        case FETCH_REQ:
                            _fetchDefault(&reqs[i]);
                            break;
                        case INSERT_REQ:
                            _insert(&reqs[i]);
                            break;
                        case DELETE_REQ:
                            _delete(&reqs[i]);
                            break;
                        default:
                            break;
                    }
                }
            }
            if (i == count) break;
            numReqsSlab += sample_size;
            mode = SENSING;
            displacement = 0;
            displacement_sq = 0;
            statReqCount = 0;
        }
        if (mode == SENSING) {
            while (numReqs < numReqsSlab && i<count) {
                for (j=0; j<10; j++, i++) {
                    switch (reqs[i].reqType) {
                        case FETCH_REQ:
                            _fetchBenchmark(&reqs[i]);
                            break;
                        case INSERT_REQ:
                            _insert(&reqs[i]);
                            break;
                        case DELETE_REQ:
                            _delete(&reqs[i]);
                            break;
                        default:
                            break;
                    }
                }
            }
            if (i == count) break;
            // calculate u1, v1, w1, and compare to u0, v0, w0
            u1 = double(displacement)/double(statReqCount);
            v1 = double(displacement_sq)/double(statReqCount-1) -
                double(displacement*displacement)/(double(statReqCount)*double(statReqCount-1));
            w1 = sqrt(((-1*log(1-confidence_prct))*2*v1)/statReqCount);
            if (abs(u0-u1) > (w0+w1)) {
                // dist has shifted with 90% probability
                numReqsSlab += epochSize;
                mode = ADAPTIVE;
            } else {
                // dist has not shifted with 90% probability
                numReqsSlab += periodicity*epochSize;
                mode = DEFAULT;
            }
        }
    }
    // Handle the remaining requests countReal - count, should be < 10
    for (; i<countReal; i++) {
        switch (reqs[i].reqType) {
            case FETCH_REQ:
                _fetchDefault(&reqs[i]);
                break;
            case INSERT_REQ:
                _insert(&reqs[i]);
                break;
            case DELETE_REQ:
                _delete(&reqs[i]);
                break;
            default:
                break;
        }
    }
    getMetricsEnd(m);
    m.displacement = displacementMetric - m.displacement;
    return m;
}

void Hashmap::rehash() {
    if (cardinality < 1.5*hmsize && cardinality > 0.5*hmsize) {
        return;
    }
    cout << "Rehashing triggered at numReqs " << this->numReqs << " cardinality: " << this->cardinality <<  endl;
    KV* old_entries = entries;
    KV** old_dict = dict;
    Acc* old_accesses = accesses;
    Acc** old_accesses_dict= accessesDict;
    ulong old_hmsize = hmsize;
    this->hashpower = _getHashpower();
    hmsize = pow(2, hashpower);
    epochSize = hmsize*epoch_size_factor;
    dict = (KV**)malloc(sizeof(KV*)*hmsize);
    entries = (KV*)malloc(sizeof(KV)*hmsize*20);
    accessesDict = (Acc**)malloc(sizeof(Acc*)*hmsize);
    accesses = (Acc*)malloc(sizeof(Acc)*hmsize*20);
    memset(this->dict, 0, sizeof(KV*)*hmsize);
    memset(this->entries, 0, sizeof(KV)*hmsize*20);
    memset(this->accessesDict, 0, sizeof(Acc*)*hmsize);
    memset(this->accesses, 0, sizeof(Acc)*hmsize*20);
    entriesOffset = 0;
    accessesOffset = 0;

    KV* ptr;
    for (ulong i=0; i<old_hmsize; i++) {
        ptr = old_dict[i];
        while (ptr != NULL) {
            _setFinalEnd(ptr->key, ptr->value);
            ptr = ptr->next;
        }
    }
    std::free(old_entries);
    std::free(old_dict);
    std::free(old_accesses_dict);
    std::free(old_accesses);
}

void Hashmap::free() {
    std::free(dict);
    std::free(entries);
    std::free(accessesDict);
    std::free(accesses);
}

inline ulong Hashmap::_random() {
    ulong r = (ulong)random();
    r = r<<31;
    r = r + (ulong)random();
    return r;
}

inline ulong Hashmap::_murmurHash(ulong h) {
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccd;
    h ^= h >> 33;
    h *= 0xc4ceb9fe1a85ec53;
    h ^= h >> 33;
    h = h >> (64 - hashpower);
    return h;
}

inline void Hashmap::_fetchDefault(HashmapReq *r) {
    ulong h = _murmurHash(r->key);
    KV* ptr = dict[h];
    while (ptr && ptr->key != r->key) {
#if _COUNT_DISP_
        displacementMetric += 1;
#endif
        ptr = ptr->next;
    }
    if (ptr == NULL) return;
#if _COUNT_DISP_
    displacementMetric += 1;
#endif
    r->value = ptr->value;
    numReqs += 1;
}

inline void Hashmap::_fetchBenchmark(HashmapReq *r) {
    ulong h = _murmurHash(r->key);
    KV* ptr = dict[h];
    ulong disp = 0;
    while (ptr && ptr->key != r->key) {
#if _COUNT_DISP_
        displacementMetric += 1;
#endif
        disp += 1;
        ptr = ptr->next;
    }
    if (ptr == NULL) return;
#if _COUNT_DISP_
    displacementMetric += 1;
#endif
    r->value = ptr->value;
    displacement += disp;
    displacement_sq += disp*disp;
    numReqs += 1;
    statReqCount += 1;
}

inline void Hashmap::_fetchAdaptive(HashmapReq *r) {
    ulong h = _murmurHash(r->key);
    KV* ptr = dict[h];
    KV* min_access_entry = ptr;
    Acc* aptr = accessesDict[h];
    Acc* aptr_prev = NULL;
    if (ptr && !aptr) {
        accessesDict[h] = &accesses[accessesOffset];
        aptr = &accesses[accessesOffset];
        accessesOffset += 1;
    }
    Acc* min_access_ptr = aptr;
    while (ptr && ptr->key != r->key) {
        if (!aptr) {
            aptr_prev->next = &accesses[accessesOffset];
            aptr = &accesses[accessesOffset];
            accessesOffset += 1;
        }
        if (aptr->accesses < min_access_ptr->accesses) {
            min_access_ptr = aptr;
            min_access_entry = ptr;
        }
#if _COUNT_DISP_
        displacementMetric += 1;
#endif
        ptr = ptr->next;
        aptr_prev = aptr;
        aptr = aptr->next;
    }
    if (ptr == NULL) return;
    if (!aptr) {
        // aptr_prev is null only if this is the first entry in the chain
        // but we have taken care of that case, aptr can't be null
        aptr_prev->next = &accesses[accessesOffset];
        aptr = &accesses[accessesOffset];
        accessesOffset += 1;
    }
#if _COUNT_DISP_
    displacementMetric += 1;
#endif
    r->value = ptr->value;
    numReqs += 1;
    aptr->accesses += 1;
    if (aptr->accesses > min_access_ptr->accesses) {
        // exchange
        ulong buf;
        uint8_t abuf;

        // key
        buf = ptr->key;
        ptr->key = min_access_entry->key;
        min_access_entry->key = buf;

        // value
        buf = ptr->value;
        ptr->value = min_access_entry->value;
        min_access_entry->value = buf;

        // accesses
        abuf = aptr->accesses;
        aptr->accesses = min_access_ptr->accesses;
        min_access_ptr->accesses = abuf;
    }
}

inline void Hashmap::_insert(HashmapReq *r) {
    ulong h = _murmurHash(r->key);
    KV* ptr = dict[h];
    while (ptr && ptr->key != r->key) {
        ptr = ptr->next;
    }
    if (ptr==NULL) {
        _setFinal(r->key, r->value);
        cardinality += 1;
        return;
    }
    // else
    ptr->value = r->value;
    numReqs += 1;
}

inline void Hashmap::_delete(HashmapReq *r) {
    ulong h = _murmurHash(r->key);
    KV *prev, *cur;
    cur = prev = dict[h];
    int steps = 1;
    while (cur && cur->key != r->key) {
        prev = cur;
        // aprev = acur;
        cur = cur->next;
        steps += 1;
        // acur = acur->next;
    }
    if (cur == NULL) return;
    if (prev != cur) {
        prev->next = cur->next;
        // aprev->next = acur->next;
    } else {
        dict[h] = cur->next;
        // accessesDict[h] = acur->next;
    }
    cardinality -= 1;

    if (mode == ADAPTIVE) {
        Acc *aprev, *acur; // corresponding pointers in the accesses hm
        aprev = acur = accessesDict[h]; // acc ptr doesn't exist
        while (acur && steps) {
            steps -= 1;
            aprev = acur;
            acur = acur->next;
        }
        if (steps || !acur) return; // acc ptr doesn't exist
        if (aprev != acur) {
            aprev->next = acur->next;
        } else {
            accessesDict[h] = acur->next;
        }
    }
}

inline void Hashmap::_setFinal(ulong key, ulong value) {
    entries[entriesOffset].key = key;
    entries[entriesOffset].value = value;
    ulong h = _murmurHash(key);
    entries[entriesOffset].next = dict[h];
    dict[h] = &entries[entriesOffset];
    entriesOffset += 1;
    if (mode == ADAPTIVE) {
        accesses[accessesOffset].next = accessesDict[h];
        accessesDict[h] = &accesses[accessesOffset];
        accessesOffset += 1;
    }
}

// Used only while rehashing, don't use elsewhere
// Inserts at the end of the chain, not at the start
// Only while rehashing we eagerly mirror accessesDict to follow the structure of dict
inline void Hashmap::_setFinalEnd(ulong key, ulong value) {
    entries[entriesOffset].key = key;
    entries[entriesOffset].value = value;
    ulong h = _murmurHash(key);
    KV* ptr = dict[h];
    Acc* aptr = accessesDict[h];
    entries[entriesOffset].next = NULL;
    accesses[accessesOffset].next = NULL;
    if (ptr == NULL) {
        dict[h] = &entries[entriesOffset];
        accessesDict[h] = &accesses[accessesOffset];
    } else {
        while (ptr->next != NULL) {
            ptr = ptr->next;
            aptr = aptr->next;
        }
        ptr->next = &entries[entriesOffset];
        aptr->next = &accesses[accessesOffset];
    }
    entriesOffset += 1;
    accessesOffset += 1;
}

inline ulong Hashmap::_getTimeDiff(struct timespec startTime, struct timespec endTime) {
    return (ulong)((endTime.tv_sec - startTime.tv_sec)*1000000 +
        double(endTime.tv_nsec - startTime.tv_nsec)/1000);
}

inline int Hashmap::_getHashpower() {
    int hashpower = 0;
    while (pow(2, hashpower) < this->cardinality/1.5) {
        hashpower += 1;
    }
    return hashpower;
}

inline void Hashmap::_resetAccesses() {
    memset(this->accesses, 0, sizeof(Acc)*(accessesOffset));
    memset(this->accessesDict, 0, sizeof(Acc*)*hmsize);
    accessesOffset = 0;
}

inline void Hashmap::_clearCache() {
#if _INTEL_INTRINSICS_
    void *ptr = this->accesses;
    ulong sz = sizeof(Acc)*(accessesOffset);
    ulong n = sz/64; // cache-line size
    for (ulong i=0; i<n; i++) {
        _mm_clflushopt((char*)ptr + 64*i);
    }

    ptr = this->accessesDict;
    sz = sizeof(Acc*)*hmsize;
    n = sz/64;
    for (ulong i=0; i<n; i++) {
        _mm_clflushopt((char*)ptr + 64*i);
    }
#else
    return;
#endif
}
