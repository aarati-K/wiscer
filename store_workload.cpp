#ifdef STORE

#include "store_workload.h"

StoreWorkload::StoreWorkload() {
    keys_file.open("keys", ios::out | ios::binary);
    ops_file.open("ops", ios::out | ios::binary);
}

void StoreWorkload::initHashpower(int hashpower) {}

/*
 * Store keys to a binary file with 64-bit unsigned long integers.
 * First is the total number of keys, followed by the keys.
*/
void StoreWorkload::bulkLoad(ulong *keys, ulong num_keys) {
#if _SORT_KEYS_
    Metrics m;
    clock_gettime(CLOCK_MONOTONIC, &(m.startTime));
    _heapSort(keys, num_keys);
    clock_gettime(CLOCK_MONOTONIC, &(m.endTime));
    cout << "Sort time (us): " << getTimeDiff(m.startTime, m.endTime) << endl;
#endif
    for (ulong i=0; i<num_keys; i++) {
        keys_file.write((char*)&keys[i], sizeof(ulong));
    }
}

/*
 * Store ops to a binary file.
 * Format is opcode (char) followed by the unsigned long 64-bit key.
 */
Metrics StoreWorkload::processRequests(HashmapReq *reqs, ulong num_reqs) {
    Metrics m;
    clock_gettime(CLOCK_MONOTONIC, &(m.startTime));
    for (ulong i=0; i<num_reqs; i++) {
        switch (reqs[i].reqType) {
            case FETCH_REQ:
                ops_file.write((char*)&fetchOpcode, sizeof(char));
                break;
            case INSERT_REQ:
                ops_file.write((char*)&insertOpcode, sizeof(char));
                break;
            case DELETE_REQ:
                ops_file.write((char*)&deleteOpcode, sizeof(char));
                break;
            default:
                break;
        }
        ops_file.write((char*)&reqs[i].key, sizeof(ulong));
    }
    clock_gettime(CLOCK_MONOTONIC, &(m.endTime));
    m.timeElapsedus = getTimeDiff(m.startTime, m.endTime);
    return m;
}

void StoreWorkload::rehash() {}

void StoreWorkload::free() {
    keys_file.close();
    ops_file.close();
}

void StoreWorkload::_heapify(ulong arr[], long n, long i) {
    long largest = i; // Initialize largest as root
    long l = 2 * i + 1; // left = 2*i + 1
    long r = 2 * i + 2; // right = 2*i + 2

    if (l < n && arr[l] > arr[largest])
        largest = l;

    if (r < n && arr[r] > arr[largest])
        largest = r;

    if (largest != i) {
        swap(arr[i], arr[largest]);
        _heapify(arr, n, largest);
    }
}

void StoreWorkload::_heapSort(ulong arr[], long n) {
    for (long i = n / 2 - 1; i >= 0; i--)
        _heapify(arr, n, i);

    for (long i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        _heapify(arr, i, 0);
    }
}

#endif // STORE
