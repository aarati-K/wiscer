#include "workload.h"
#include <cstdlib>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Usage: ./benchmark.out workload_file_name" << endl;
        exit(1);
    }
    Workload *w = new Workload(argv[1]);
    w->printParams();
    w->run();
    return 0;
}
