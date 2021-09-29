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

void StoreWorkload::_swap(ulong *a, ulong *b) {
    ulong tmp = *a;
    *a = *b;
    *b = tmp;
}

 void StoreWorkload::_heapify(ulong arr[], ulong n, ulong i) {
    ulong max = i; //Initialize max as root
    ulong leftChild = 2 * i + 1;
    ulong rightChild = 2 * i + 2;
  
    //If left child is greater than root
    if (leftChild < n && arr[leftChild] > arr[max])
      max = leftChild;
  
    //If right child is greater than max
    if (rightChild < n && arr[rightChild] > arr[max])
      max = rightChild;
  
    //If max is not root
    if (max != i) {
      swap(arr[i], arr[max]);
      _heapify(arr, n, max); //heapify the affected sub-tree recursively
    }
  }
  
  //Main function to perform heap sort
void StoreWorkload::_heapSort(ulong arr[], ulong n) {
    //Rearrange array (building heap)
    for (ulong i = n / 2 - 1; i >= 0; i--)
      _heapify(arr, n, i);
  
    //Extract elements from heap one by one
    for (ulong i = n - 1; i >= 0; i--) {
      swap(arr[0], arr[i]); //Current root moved to the end
  
      _heapify(arr, i, 0); //calling max heapify on the heap reduced
    }
}
