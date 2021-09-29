#include "store_workload.h"

StoreWorkload::StoreWorkload() {
    keys_file.open("keys_uint64", ios::out | ios::binary);
    ops_file.open("ops", ios::out | ios::binary);
}

void StoreWorkload::initHashpower(int hashpower) {}

void StoreWorkload::bulkLoad(ulong *keys, ulong num_keys) {
    // Add code for sorting
    keys_file.write((char*)&num_keys, sizeof(ulong));
    for (ulong i=0; i<num_keys; i++) {
        keys_file.write((char*)&keys[i], sizeof(ulong));
    }
}

Metrics StoreWorkload::processRequests(HashmapReq *reqs, ulong num_reqs) {
    Metrics m;
    return m;
}

void StoreWorkload::rehash() {}

void StoreWorkload::free() {}
