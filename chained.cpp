#include "chained.h"

Hashmap::Hashmap() {
    this->hashpower = -1; // not initialized
    this->hmsize = 0;
    this->cardinality = 0;
    this->dict = NULL;
    this->entries = NULL;
    this->entriesOffset = 0;
}

void Hashmap::initHashpower(int hashpower) {
    this->hashpower = hashpower;
    this->hmsize = pow(2, hashpower);
    this->dict = (KV**)malloc(sizeof(KV*)*hmsize);
    this->entries = (KV*)malloc(sizeof(KV)*hmsize*20); // safety factor
    memset(this->dict, 0, sizeof(KV*)*hmsize);
    memset(this->entries, 0, sizeof(KV)*hmsize*20);
    entriesOffset = 0;
}

void Hashmap::bulkLoad(ulong *keys, ulong num_keys) {
    for (ulong i=0; i<num_keys; i++) {
        _setFinal(keys[i], _random());
    }
    this->cardinality = num_keys;
}

Metrics Hashmap::processRequests(HashmapReq *reqs, ulong count) {
    Metrics m;
    m.displacement = displacement;
    getMetricsStart(m);
    for (ulong i=0; i<count; i++) {
        switch (reqs[i].reqType) {
            case FETCH_REQ:
                _fetch(&reqs[i]);
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
    m.displacement = displacement - m.displacement;
    return m;
}

void Hashmap::rehash() {
    if (cardinality < 1.5*hmsize && cardinality > 0.5*hmsize) {
        return;
    }
    cout << "Rehashing triggered at numReqs " << this->numReqs << " cardinality: " << this->cardinality <<  endl;
    KV* old_entries = entries;
    KV** old_dict = dict;
    ulong old_hmsize = hmsize;
    this->hashpower = _getHashpower();
    hmsize = pow(2, hashpower);
    dict = (KV**)malloc(sizeof(KV*)*hmsize);
    entries = (KV*)malloc(sizeof(KV)*hmsize*20);
    memset(this->dict, 0, sizeof(KV*)*hmsize);
    memset(this->entries, 0, sizeof(KV)*hmsize*20);
    entriesOffset = 0;

    KV* ptr;
    for (ulong i=0; i<old_hmsize; i++) {
        ptr = old_dict[i];
        while (ptr != NULL) {
            _setFinal(ptr->key, ptr->value);
            ptr = ptr->next;
        }
    }
    std::free(old_entries);
    std::free(old_dict);
}

void Hashmap::free() {
    std::free(dict);
    std::free(entries);
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

inline void Hashmap::_fetch(HashmapReq *r) {
    ulong h = _murmurHash(r->key);
    KV* ptr = dict[h];
    while (ptr && ptr->key != r->key) {
#if _COUNT_DISP_
        displacement += 1;
#endif
        ptr = ptr->next;
    }
    if (ptr == NULL) return;
#if _COUNT_DISP_
    displacement += 1;
#endif
    r->value = ptr->value;
    numReqs += 1;
}

inline void Hashmap::_insert(HashmapReq *r) {
    ulong h = _murmurHash(r->key);
    KV* ptr = dict[h];
    while (ptr && ptr->key != r->key) {
        ptr = ptr->next;
    }
    if (ptr != NULL) {
        ptr->value = r->value;
        return;
    }
    // else, insert
    _setFinal(r->key, r->value);
    cardinality += 1;
    numReqs += 1;
}

inline void Hashmap::_delete(HashmapReq *r) {
    ulong h = _murmurHash(r->key);
    KV *prev, *cur;
    cur = prev = dict[h];
    while (cur != NULL && cur->key != r->key) {
        prev = cur;
        cur = cur->next;
    }
    if (cur == NULL) return;
    if (prev != cur) {
        prev->next = cur->next;
    } else {
        dict[h] = cur->next;
    }
    cardinality -= 1;
    numReqs += 1;
}

inline void Hashmap::_setFinal(ulong key, ulong value) {
    entries[entriesOffset].key = key;
    entries[entriesOffset].value = value;
    ulong h = _murmurHash(key);
    entries[entriesOffset].next = dict[h];
    dict[h] = &entries[entriesOffset];
    entriesOffset += 1;
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
