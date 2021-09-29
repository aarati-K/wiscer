#include "store_workload.h"

StoreWorkload::StoreWorkload() {
    keys_file.open("keys_uint64", ios::out | ios::binary);
    ops_file.open("ops", ios::out | ios::binary);
}

void StoreWorkload::initHashpower(int hashpower) {}

void StoreWorkload::bulkLoad(ulong *keys, ulong num_keys) {
    Metrics m;
    clock_gettime(CLOCK_MONOTONIC, &(m.startTime));
    _heapSort(keys, num_keys);
    clock_gettime(CLOCK_MONOTONIC, &(m.endTime));
    cout << "Sort time (us): " << getTimeDiff(m.startTime, m.endTime) << endl;
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

void StoreWorkload::free() {
    keys_file.close();
    ops_file.close();
}

// Helper functions for sorting

// To heapify a subtree rooted with node i which is
// an index in arr[]. n is size of heap
void StoreWorkload::_heapify(ulong *arr, ulong n, ulong i)
{
    ulong largest = i; // Initialize largest as root
    ulong l = 2 * i + 1; // left = 2*i + 1
    ulong r = 2 * i + 2; // right = 2*i + 2
 
    // If left child is larger than root
    if (l < n && arr[l] > arr[largest])
        largest = l;
 
    // If right child is larger than largest so far
    if (r < n && arr[r] > arr[largest])
        largest = r;
 
    // If largest is not root
    if (largest != i) {
        swap(arr[i], arr[largest]);
 
        // Recursively heapify the affected sub-tree
        _heapify(arr, n, largest);
    }
}
 
// main function to do heap sort
void StoreWorkload::_heapSort(ulong *arr, ulong n)
{
    // Build heap (rearrange array)
    for (ulong i = n / 2 - 1; i >= 0; i--)
        _heapify(arr, n, i);
 
    // One by one extract an element from heap
    for (ulong i = n - 1; i > 0; i--) {
        // Move current root to end
        swap(arr[0], arr[i]);
 
        // call max heapify on the reduced heap
        _heapify(arr, i, 0);
    }
}
